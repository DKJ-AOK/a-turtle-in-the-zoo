//
// Created by danie on 28/01/2026.
//

#ifndef A_TURTLE_IN_THE_ZOO_WORLD_H
#define A_TURTLE_IN_THE_ZOO_WORLD_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <queue>
#include <vector>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <shared_mutex>

#include "Physics.h"

struct MeshData;
struct ChunkData;
enum BlockType : int; // Forward declare the enum
class Chunk;
class Texture;
class Shader;
class Camera;

struct CompletedChunk {
    glm::ivec3 pos;
    std::shared_ptr<Chunk> chunk;
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

    bool isFaceVisible(glm::ivec3 worldPos, glm::ivec3 dir, BlockType currentBlock);

    // Collision Check
    bool checkCollision(const AABB& playerBox, AABB& cubeOutBox) const;
    static bool intersect(const AABB& a, const AABB& b);
    static AABB getBlockAABB(int x, int y, int z);

    // Raycast
    RaycastResult raycast(glm::vec3 origin, glm::vec3 direction, float maxDistance) const;

private:
    std::map<std::pair<int, int>, ChunkData> chunks;
    std::vector<Texture> textures;
    std::uint32_t seed;
    int renderDistance = 16;
    std::queue<glm::ivec3> chunksToGenerate;
    mutable std::shared_mutex chunksMutex;

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