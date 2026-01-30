#include "../Header Files/World.h"

World::World(std::uint32_t seed, const std::vector<Texture> &textures) : textures(textures), seed(seed), running(true) {
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
        delete pair.second.chunk;
        delete pair.second.mesh;
    }
    chunks.clear();

    // Clean up any leftover data in the completed queue
    // (In case the game closed before they were moved to the map)
    for (auto& item : completedChunks) {
        delete item.chunk;
        delete item.meshData;
    }
}

void World::updateChunks(const glm::vec3 cameraPos) {
    // Convert camera position to chunk coordinates
    int camX = static_cast<int>(std::floor(cameraPos.x / (Chunk::SIZE_X_Z * 0.4f)));
    int camZ = static_cast<int>(std::floor(cameraPos.z / (Chunk::SIZE_X_Z * 0.4f)));

    int radius = renderDistance / 2;

    // 1. LOADING: Add new chunks around the camera
    for (int x = camX - radius; x <= camX + radius; x++) {
        for (int z = camZ - radius; z <= camZ + radius; z++) {
            // Only create if it doesn't exist
            if (chunks.find({x, z}) == chunks.end()) {
                AddChunkToGenerate({x, z});
            }
        }
    }

    // 2. UNLOADING: Remove chunks that are too far away
    for (auto it = chunks.begin(); it != chunks.end(); ) {
        const int dx = it->first.first - camX;
        const int dz = it->first.second - camZ;

        // If distance is greater than radius + buffer (to prevent flickering)
        if (std::abs(dx) > radius + 2 || std::abs(dz) > radius + 2) {
            delete it->second.chunk;
            delete it->second.mesh;
            it = chunks.erase(it);   // Remove from map and get next iterator
        } else {
            ++it;
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
        const auto newMesh = new Mesh(item.meshData->vertices, item.meshData->indices, textures);
        delete item.meshData;

        auto& entry = chunks[{item.pos.x, item.pos.z}];
        delete entry.mesh;

        entry.chunk = item.chunk;
        entry.mesh = newMesh;
    }
}

void World::AddChunkToGenerate(glm::ivec2 pos) {
    std::lock_guard lock(queueMutex);
    pendingPositions.emplace(pos.x, 0, pos.y);
    cv.notify_one();

    // Make sure same chunk does not get initialized again
    chunks[{pos.x, pos.y}] = ChunkData(nullptr, nullptr);
}

void World::draw(Shader& shader, Camera& camera) const {
    glUniform1i(glGetUniformLocation(shader.ID, "useInstancing"), 0);
    auto model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    for (auto& pair : chunks) {
        // Ensure the mesh exists before drawing
        if (pair.second.mesh) {
            pair.second.mesh->Draw(shader, camera);
        }
    }
}

void World::updateRenderDistance(const int newDistance) {
    renderDistance = newDistance;
}

void World::addBlockAtWorldPosition(const glm::ivec3 pos, const BlockType type) {
    auto chunkPos = glm::ivec2(
        std::floor(static_cast<float>(pos.x) / Chunk::SIZE_X_Z),
        std::floor(static_cast<float>(pos.z) / Chunk::SIZE_X_Z));

    const auto it = chunks.find({chunkPos.x, chunkPos.y});
    if (it != chunks.end() && it->second.chunk) {
        // Update the block data
        it->second.chunk->addBlockAtWorldPosition(pos, type);
        it->second.isModified = true;

        // Add to the worker queue to rebuild the mesh
        std::lock_guard lock(queueMutex);

        pendingPositions.emplace(chunkPos.x, 1, chunkPos.y); // Use Y=1 as a 'Rebuild' flag
        cv.notify_one();
    }
}

BlockType World::removeBlockAtWorldPosition(glm::ivec3 pos) {
    auto chunkPos = glm::ivec2(
        std::floor(static_cast<float>(pos.x) / Chunk::SIZE_X_Z),
        std::floor(static_cast<float>(pos.z) / Chunk::SIZE_X_Z));

    const auto it = chunks.find({chunkPos.x, chunkPos.y});
    if (it != chunks.end() && it->second.chunk) {
        const auto blockType = it->second.chunk->getBlockTypeAtWorldPosition(pos);
        addBlockAtWorldPosition(pos, AIR);
        return blockType;
    }
    return AIR;
}

BlockType World::getBlockTypeAtWorldPosition(glm::ivec3 pos) const {
    auto chunkPos = glm::ivec2(
        std::floor(static_cast<float>(pos.x) / Chunk::SIZE_X_Z),
        std::floor(static_cast<float>(pos.z) / Chunk::SIZE_X_Z));

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
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this] { return !pendingPositions.empty() || !running; });
            if (!running) break;
            task = pendingPositions.front();
            pendingPositions.pop();
        }

        Chunk* targetChunk = nullptr;
        if (task.y == 0) {
            // Task: Load New Chunk
            targetChunk = new Chunk(glm::ivec3(task.x, 0, task.z), seed);
        } else {
            // Task: Rebuild Existing Chunk
            auto it = chunks.find({task.x, task.z});
            if (it != chunks.end()) targetChunk = it->second.chunk;
        }

        if (targetChunk) {
            MeshData* data = targetChunk->generateMesh();
            std::lock_guard lock(completedMutex);
            completedChunks.push_back({glm::ivec3(task.x, task.y, task.z), targetChunk, data});
        }
    }
}
