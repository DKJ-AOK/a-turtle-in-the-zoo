#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm/glm.hpp>
#include <PerlinNoise.hpp>
#include <memory>
#include "Mesh.h"

class World;

enum BlockType : int {
    AIR = 0,
    GRASS = 1,
    DIRT = 2,
    STONE = 3,
    SAND = 4,
    SNOWY_GRASS = 5,
    WATER = 6
};

enum Biome {
    PLANES = 0,
    DESSERT = 1,
    MOUNTAINS = 2,
    SNOWY_TAIGA = 3
};

struct UVRect {
    float uStart, uEnd, vStart, vEnd;
};

struct MeshData {
    std::vector<Vertex> opaqueVertices;
    std::vector<GLuint> opaqueIndices;
    std::vector<Vertex> transparentVertices;
    std::vector<GLuint> transparentIndices;
};

struct BiomeSettings {
    float frequency;
    int octaves;
    int baseHeight;
};

struct BiomeWeights {
    float plains;
    float mountain;
    float desert;
    float snowyTaiga;
};

class Chunk {
public:
    static constexpr int SIZE_X_Z = 16;
    static constexpr int SIZE_Y = 256;
    static constexpr float BLOCK_SCALE = 0.4f;
    BlockType blocks[SIZE_X_Z][SIZE_Y][SIZE_X_Z]{};
    glm::ivec3 position;

    explicit Chunk(glm::ivec3 pos, std::uint32_t seed = 0);
    [[nodiscard]] MeshData* generateMesh(World& world) const;
    [[nodiscard]] MeshData* generateTransparentMesh(World& world) const;

    void addBlockAtWorldPosition(glm::ivec3 pos, BlockType type);
    [[nodiscard]] BlockType getBlockTypeAtWorldPosition(glm::ivec3 pos) const;

private:
    int seaLevel = 20;

    static void addFace(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, glm::vec3 pos, int faceDir, BlockType type);
    static UVRect getUVsForCoordinates(int column, int row);
    static UVRect getUVs(BlockType type, int faceDir);

    [[nodiscard]] int getNoiseHeightAtWorldPosition(glm::ivec2 pos, uint32_t seed, const BiomeWeights& weights) const;
    [[nodiscard]] static BiomeWeights getBiomeWeightsAtWorldPosition(glm::ivec2 pos, std::uint32_t seed);

    BiomeSettings plains   = { 0.025f, 4, 20 };
    BiomeSettings mountain = { 0.050f, 6, 60 };
    BiomeSettings desert   = { 0.020f, 3, 15 };
    BiomeSettings snowyTaiga = { 0.025f, 4, 25 };
};

struct ChunkData {
    std::shared_ptr<Chunk> chunk;    // Pointer to the Chunk object (contains block data)
    Mesh* opaqueMesh;      // Pointer to the Mesh object (used for rendering)
    Mesh* transparentMesh;      // Pointer to the Mesh object (used for rendering)
    bool isModified; // true if the user changed blocks (useful for saving to disk)

    // Constructor for convenience
    explicit ChunkData(std::shared_ptr<Chunk> c = nullptr, Mesh* om = nullptr, Mesh* tm = nullptr)
        : chunk(std::move(c)), opaqueMesh(om), transparentMesh(tm), isModified(false) {
    }
};

#endif
