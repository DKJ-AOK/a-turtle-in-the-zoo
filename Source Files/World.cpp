#include "../Header Files/World.h"

void World::updateChunks(const glm::vec3 cameraPos) {
    // Convert camera position to chunk coordinates
    int camX = static_cast<int>(std::floor(cameraPos.x / (16 * 0.4f)));
    int camZ = static_cast<int>(std::floor(cameraPos.z / (16 * 0.4f)));

    int radius = renderDistance / 2;

    // 1. LOADING: Add new chunks around the camera
    for (int x = camX - radius; x <= camX + radius; x++) {
        for (int z = camZ - radius; z <= camZ + radius; z++) {
            // Only create if it doesn't exist
            if (chunks.find({x, z}) == chunks.end()) {
                auto newChunk = new Chunk(glm::ivec3(x, 0, z), seed);
                Mesh* newMesh = newChunk->generateMesh(textures);
                chunks[{x, z}] = ChunkData(newChunk, newMesh);
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

void World::addBlock(const glm::ivec3 pos, const BlockType type) {
    auto chunkPos = glm::ivec2(std::floor(pos.x / 16.0f), std::floor(pos.z / 16.0f));
    auto& chunkData = chunks[{chunkPos.x, chunkPos.y}];
    chunkData.chunk->blocks[pos.x % 16][pos.y][pos.z % 16] = type;
    chunkData.mesh = chunkData.chunk->generateMesh(textures);
    chunkData.isModified = true;
}

void World::removeBlock(glm::ivec3 pos) {
    addBlock(pos, AIR);
}
