#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm/glm.hpp>
#include <PerlinNoise.hpp>
#include "Mesh.h"

enum BlockType {
    AIR = 0,
    GRASS = 1,
    DIRT = 2,
    STONE = 3,
    SAND = 4
};

enum Biome {
    PLANES = 0,
    DESSERT = 1,
    MOUNTAINS = 2
};

struct UVRect {
    float uStart, uEnd, vStart, vEnd;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

class Chunk {
public:
    static constexpr int SIZE_X_Z = 16;
    static constexpr int SIZE_Y = 256;
    BlockType blocks[SIZE_X_Z][SIZE_Y][SIZE_X_Z]{};
    glm::ivec3 position;

    explicit Chunk(glm::ivec3 pos, std::uint32_t seed = 0);
    [[nodiscard]] MeshData* generateMesh() const;

    void addBlockAtWorldPosition(glm::ivec3 pos, BlockType type);
    [[nodiscard]] BlockType getBlockTypeAtWorldPosition(glm::ivec3 pos) const;

private:
    static void addFace(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, glm::vec3 pos, int faceDir, BlockType type);
    static UVRect getUVs(int column, int row);

    [[nodiscard]] int getNoiseHeightAtWorldPosition(glm::ivec2 pos, uint32_t seed, Biome biome) const;
    [[nodiscard]] Biome getBiomeAtWorldPosition(glm::ivec2 pos, std::uint32_t seed) const;
};

struct ChunkData {
    Chunk* chunk;    // Pointer to the Chunk object (contains block data)
    Mesh* mesh;      // Pointer to the Mesh object (used for rendering)
    bool isModified; // Optional: true if the user changed blocks (useful for saving to disk)

    // Constructor for convenience
    explicit ChunkData(Chunk* c = nullptr, Mesh* m = nullptr)
        : chunk(c), mesh(m), isModified(false) {}
};

#endif
