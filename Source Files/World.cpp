#include "../Header Files/World.h"
#include "../Header Files/Chunk.h"

World::World(const std::uint32_t seed, const std::vector<Texture> &textures) : textures(textures), seed(seed) {
    // THIS LINE IS ESSENTIAL:
    workerThread = std::thread(&World::workerLoop, this);
}

World::~World() {
    // Tell the worker thread to stop
    running = false;
    cv.notify_all(); // Wake up the worker thread if it's sleeping

    // Wait for the thread to actually finish
    if (workerThread.joinable()) {
        workerThread.join();
    }

    // Clean up all loaded chunks and meshes
    for (auto& pair : chunks) {
        delete pair.second.opaqueMesh;
        delete pair.second.transparentMesh;
    }
    chunks.clear();

    // Clean up any leftover data in the completed queue
    // (In case the game closed before they were moved to the map)
    for (auto& item : completedChunks) {
        delete item.meshData;
    }
}

void World::updateChunks(const glm::vec3 cameraPos) {
    // Convert camera position to chunk coordinates
    const int camX = static_cast<int>(std::floor(cameraPos.x / (Chunk::SIZE_X_Z * Chunk::BLOCK_SCALE)));
    const int camZ = static_cast<int>(std::floor(cameraPos.z / (Chunk::SIZE_X_Z * Chunk::BLOCK_SCALE)));

    const int radius = renderDistance / 2;

    // 1. LOADING: Add new chunks around the camera
    for (int x = camX - radius; x <= camX + radius; x++) {
        for (int z = camZ - radius; z <= camZ + radius; z++) {
            // Only create if it doesn't exist
            bool exists;
            {
                std::lock_guard lock(chunksMutex);
                exists = (chunks.contains({x, z}));
            }
            if (!exists) {
                AddChunkToGenerate({x, z});
            }
        }
    }

    // 2. UNLOADING: Remove chunks that are too far away
    {
        std::lock_guard lock(chunksMutex);
        std::lock_guard qLock(queueMutex); // Lock queue too to prevent worker from getting a chunk we are about to delete
        for (auto it = chunks.begin(); it != chunks.end(); ) {
            const int dx = it->first.first - camX;
            const int dz = it->first.second - camZ;

            // If distance is greater than radius + buffer (to prevent flickering)
            if (std::abs(dx) > radius + 2 || std::abs(dz) > radius + 2) {
                // Since we use shared_ptr, it is safe to erase it from the map.
                // If a worker thread is currently meshing it, it will keep a reference.
                
                delete it->second.opaqueMesh;
                delete it->second.transparentMesh;
                it = chunks.erase(it);   // Remove from map and get next iterator
            } else {
                ++it;
            }
        }
    }

    std::vector<CompletedChunk> localCompleted;
    {
        std::lock_guard lock(completedMutex);
        localCompleted = std::move(completedChunks);
        completedChunks.clear();
    }

    for (auto& item : localCompleted) {
        // This part happens on the MAIN THREAD (OpenGL safe!)
        const auto newOpaqueMesh = new Mesh(item.meshData->opaqueVertices, item.meshData->opaqueIndices, textures);
        const auto newTransparentMesh = new Mesh(item.meshData->transparentVertices, item.meshData->transparentIndices, textures);
        delete item.meshData;

        std::lock_guard lock(chunksMutex);
        if (auto it = chunks.find({item.pos.x, item.pos.z}); it != chunks.end()) {
            auto& entry = it->second;
            delete entry.opaqueMesh;
            delete entry.transparentMesh;

            entry.chunk = std::move(item.chunk);
            entry.opaqueMesh = newOpaqueMesh;
            entry.transparentMesh = newTransparentMesh;

            // If this was a new chunk, trigger rebuild of neighbors to fix boundary faces
            if (item.pos.y == 0) {
                const glm::ivec2 neighbors[] = {
                    {item.pos.x + 1, item.pos.z}, {item.pos.x - 1, item.pos.z},
                    {item.pos.x, item.pos.z + 1}, {item.pos.x, item.pos.z - 1}
                };

                for (const auto& neighborPos : neighbors) {
                    if (auto nIt = chunks.find({neighborPos.x, neighborPos.y}); nIt != chunks.end() && nIt->second.chunk) {
                        // Add to the worker queue to rebuild the mesh
                        std::lock_guard qLock(queueMutex);
                        pendingPositions.emplace(neighborPos.x, 1, neighborPos.y); // Use Y=1 as a 'Rebuild' flag
                        cv.notify_one();
                    }
                }
            }
        } else {
            // Chunk was unloaded while we were meshing it
            delete newOpaqueMesh;
            delete newTransparentMesh;
        }
    }
}

void World::AddChunkToGenerate(glm::ivec2 pos) {
    std::lock_guard qLock(queueMutex);
    pendingPositions.emplace(pos.x, 0, pos.y);
    cv.notify_one();

    // Make sure same chunk does not get initialized again
    std::lock_guard cLock(chunksMutex);
    chunks[{pos.x, pos.y}] = ChunkData(nullptr, nullptr);
}

void World::draw(Shader& shader, Camera& camera) const {
    glUniform1i(glGetUniformLocation(shader.ID, "useInstancing"), 0);
    auto model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    std::lock_guard lock(chunksMutex);

    // Opaque blocks
    for (auto& pair : chunks) {
        if (pair.second.opaqueMesh) {
            pair.second.opaqueMesh->Draw(shader, camera);
        }
    }

    // Transparent blocks
    glDepthMask(GL_FALSE); // Disable writing to depth buffer for water
    for (auto& pair : chunks) {
        if (pair.second.transparentMesh) {
            pair.second.transparentMesh->Draw(shader, camera);
        }
    }
    glDepthMask(GL_TRUE); // Re-enable for the next frame
}

bool World::isFaceVisible(const glm::ivec3 worldPos, const glm::ivec3 dir, const BlockType currentBlock) {
    const glm::ivec3 neighborPos = worldPos + dir;

    if (neighborPos.y < 0 || neighborPos.y >= Chunk::SIZE_Y) return true;

    const int chunkX = std::floor(static_cast<float>(neighborPos.x) / Chunk::SIZE_X_Z);
    const int chunkZ = std::floor(static_cast<float>(neighborPos.z) / Chunk::SIZE_X_Z);

    std::lock_guard lock(chunksMutex);
    if (const auto it = chunks.find({chunkX, chunkZ}); it != chunks.end() && it->second.chunk) {
        const auto neighborBlock = it->second.chunk->getBlockTypeAtWorldPosition(neighborPos);

        // If they are the same type of transparent block, hide the face
        if (currentBlock == neighborBlock && currentBlock == WATER) return false;
        if (std::ranges::any_of(Chunk::NO_COLLISION_BLOCKS, [&](const BlockType quadBlock)
            { return quadBlock == neighborBlock; }))
            return true;

        return false;
    }
    // If chunk is not loaded, assume it's water if we are a water block to prevent seams
    if (currentBlock == WATER) return false;
    return true;
}

void World::updateRenderDistance(const int newDistance) {
    renderDistance = newDistance;
}

void World::addBlockAtWorldPosition(const glm::ivec3 pos, const BlockType type) {
    auto chunkPos = glm::ivec2(
        std::floor(static_cast<float>(pos.x) / Chunk::SIZE_X_Z),
        std::floor(static_cast<float>(pos.z) / Chunk::SIZE_X_Z));

    std::lock_guard lock(chunksMutex);
    const auto it = chunks.find({chunkPos.x, chunkPos.y});
    if (it != chunks.end() && it->second.chunk) {
        // Update the block data
        it->second.chunk->addBlockAtWorldPosition(pos, type);
        it->second.isModified = true;

        // Add to the worker queue to rebuild the mesh
        std::lock_guard qLock(queueMutex);

        pendingPositions.emplace(chunkPos.x, 1, chunkPos.y); // Use Y=1 as a 'Rebuild' flag
        cv.notify_one();
    }
}

BlockType World::removeBlockAtWorldPosition(glm::ivec3 pos) {
    auto chunkPos = glm::ivec2(
        std::floor(static_cast<float>(pos.x) / Chunk::SIZE_X_Z),
        std::floor(static_cast<float>(pos.z) / Chunk::SIZE_X_Z));

    std::lock_guard lock(chunksMutex);
    const auto it = chunks.find({chunkPos.x, chunkPos.y});
    if (it != chunks.end() && it->second.chunk) {
        const auto blockType = it->second.chunk->getBlockTypeAtWorldPosition(pos);
        // We call the internal update directly to avoid double locking or use a helper
        it->second.chunk->addBlockAtWorldPosition(pos, AIR);
        it->second.isModified = true;
        
        std::lock_guard qLock(queueMutex);
        pendingPositions.emplace(chunkPos.x, 1, chunkPos.y); 
        cv.notify_one();

        return blockType;
    }
    return AIR;
}

BlockType World::getBlockTypeAtWorldPosition(glm::ivec3 pos) const {
    auto chunkPos = glm::ivec2(
        std::floor(static_cast<float>(pos.x) / Chunk::SIZE_X_Z),
        std::floor(static_cast<float>(pos.z) / Chunk::SIZE_X_Z));

    std::lock_guard lock(chunksMutex);
    const auto it = chunks.find({chunkPos.x, chunkPos.y});
    if (it != chunks.end() && it->second.chunk) {
        return it->second.chunk->getBlockTypeAtWorldPosition(pos);
    }
    return AIR;
}

void World::workerLoop() {
    while (running) {
        glm::ivec3 task;
        {
            std::unique_lock lock(queueMutex);
            cv.wait(lock, [this] { return !pendingPositions.empty() || !running; });
            if (!running) break;
            task = pendingPositions.front();
            pendingPositions.pop();
        }

        std::shared_ptr<Chunk> targetChunk = nullptr;
        bool isNewChunk = (task.y == 0);
        if (isNewChunk) {
            // Task: Load New Chunk
            targetChunk = std::make_shared<Chunk>(glm::ivec3(task.x, 0, task.z), seed);
        } else {
            // Task: Rebuild Existing Chunk
            std::lock_guard lock(chunksMutex);
            auto it = chunks.find({task.x, task.z});
            if (it != chunks.end() && it->second.chunk != nullptr) {
                targetChunk = it->second.chunk;
            }
        }

        if (targetChunk) {
            MeshData* data = targetChunk->generateMesh(*this);
            
            // Re-check if the chunk still exists in the map before pushing to completed
            // This prevents the race where a chunk is unloaded while the worker is meshing it.
            
            std::lock_guard lock(chunksMutex);
            auto it = chunks.find({task.x, task.z});
            if (it != chunks.end()) {
                std::lock_guard cLock(completedMutex);
                completedChunks.push_back({glm::ivec3(task.x, task.y, task.z), std::move(targetChunk), data});
            } else {
                // Chunk was unloaded while we were meshing it!
                delete data;
            }
        }
    }
}

bool World::checkCollision(const AABB &playerBox) {
    // Find min/max Coordinates in grid-units
    int minX = std::floor(playerBox.min.x / Chunk::BLOCK_SCALE);
    int maxX = std::ceil(playerBox.max.x / Chunk::BLOCK_SCALE);
    int minY = std::floor(playerBox.min.y / Chunk::BLOCK_SCALE);
    int maxY = std::ceil(playerBox.max.y / Chunk::BLOCK_SCALE);
    int minZ = std::floor(playerBox.min.z / Chunk::BLOCK_SCALE);
    int maxZ = std::ceil(playerBox.max.z / Chunk::BLOCK_SCALE);

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            for (int z = minZ; z <= maxZ; z++) {
                if (auto blockTypeAtPos = getBlockTypeAtWorldPosition({x, y, z});
                    blockTypeAtPos != BlockType::AIR &&
                    blockTypeAtPos != BlockType::WATER) {

                    // Generate AABB for the block
                    AABB blockAABB = getBlockAABB(x, y, z);

                    if (intersect(playerBox, blockAABB))
                        return true;
                }
            }
        }
    }
    return false;
}

bool World::intersect(const AABB &a, const AABB &b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

AABB World::getBlockAABB(const int x, const int y, const int z) {
    constexpr float cubeSize = Chunk::BLOCK_SCALE;

    // We assume the cube is centered in its cell
    const auto center = glm::vec3(x, y, z);
    constexpr float half = cubeSize / 2.0f;

    return { center - glm::vec3(half), center + glm::vec3(half) };
}
