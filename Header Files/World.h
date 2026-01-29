//
// Created by danie on 28/01/2026.
//

#ifndef A_TURTLE_IN_THE_ZOO_WORLD_H
#define A_TURTLE_IN_THE_ZOO_WORLD_H
#include <map>
#include <queue>
#include <vector>
#include <glm/vec3.hpp>
#include <thread>
#include <condition_variable>
#include <atomic>

#include "Chunk.h"

struct CompletedChunk {
    glm::ivec3 pos;
    Chunk* chunk;
    MeshData* meshData;
};

class World {
public:
    explicit World(std::uint32_t seed, const std::vector<Texture>& textures);
    ~World();

    void updateChunks(glm::vec3 cameraPos);
    void draw(Shader& shader, Camera& camera) const;
    void updateRenderDistance(int newDistance);
    [[nodiscard]] int getRenderDistance() const { return renderDistance; }

    void addBlockAtWorldPosition(glm::ivec3 pos, BlockType type);
    BlockType removeBlockAtWorldPosition(glm::ivec3 pos);
    BlockType getBlockTypeAtWorldPosition(glm::ivec3 pos) const;

private:
    std::map<std::pair<int, int>, ChunkData> chunks;
    std::vector<Texture> textures;
    std::uint32_t seed;
    int renderDistance = 16;
    std::queue<glm::ivec3> chunksToGenerate;
    std::mutex chunksMutex;

    void AddChunkToGenerate(glm::ivec2 pos);

    void workerLoop();  // The function the background thread runs

    // Threading tools
    std::thread workerThread;
    std::atomic<bool> running{true};
    std::condition_variable cv;

    // Queues
    std::queue<glm::ivec3> pendingPositions;
    std::vector<CompletedChunk> completedChunks;

    std::mutex queueMutex;     // Protects pendingPositions
    std::mutex completedMutex; // Protects completedChunks
};

#endif //A_TURTLE_IN_THE_ZOO_WORLD_H