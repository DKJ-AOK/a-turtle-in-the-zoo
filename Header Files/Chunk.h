#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include <vector>
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
    WATER = 6,
    GRASS_QUAD = 7,
    RED_FLOWER_QUAD = 8,
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

struct ShapeData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

struct MeshData {
    ShapeData opaqueShapes;
    ShapeData transparentShapes;
    ShapeData crossQuadShapes;
};

struct BiomeSettings {
    float frequency;
    int octaves;
    int baseHeight;
    int variation;
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
    static constexpr float BLOCK_SCALE = 1.0f;
    static constexpr BlockType NO_COLLISION_BLOCKS[] = {AIR, GRASS_QUAD, RED_FLOWER_QUAD, WATER};
    BlockType blocks[SIZE_X_Z][SIZE_Y][SIZE_X_Z]{};
    glm::ivec3 position;

    explicit Chunk(glm::ivec3 pos, std::uint32_t seed = 0);
    [[nodiscard]] MeshData* generateMesh(World& world) const;

    void addBlockAtWorldPosition(glm::ivec3 pos, BlockType type);
    [[nodiscard]] BlockType getBlockTypeAtWorldPosition(glm::ivec3 pos) const;

private:
    int seaLevel = 32;

    void addCrossQuadFaces(ShapeData& shapes, glm::vec3 pos) const;
    void addBlockFaces(World& world, ShapeData& shapes, glm::vec3 pos, BlockType blockType) const;
    static void addBlockFace(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, glm::vec3 pos, int faceDir, BlockType blockType, bool lowerHeight = false);

    static UVRect getUVsForCoordinates(int column, int row);
    static UVRect getUVs(BlockType type, int faceDir);

    [[nodiscard]] int getNoiseHeightAtWorldPosition(glm::ivec2 pos, uint32_t seed, const BiomeWeights& weights) const;
    [[nodiscard]] static BiomeWeights getBiomeWeightsAtWorldPosition(glm::ivec2 pos, std::uint32_t seed);

    BiomeSettings plains   = { 0.025f, 4, 28, 15 };
    BiomeSettings mountain = { 0.050f, 6, 50, 40,  };
    BiomeSettings desert   = { 0.020f, 3, 25, 20 };
    BiomeSettings snowyTaiga = { 0.025f, 4, 26, 20 };
};

struct ChunkData {
    std::shared_ptr<Chunk> chunk;       // Pointer to the Chunk object (contains block data)
    Mesh* opaqueMesh;                   // Pointer to the Mesh object (used for rendering)
    Mesh* transparentMesh;              // Pointer to the Mesh object (used for rendering transparent objects, e.g. water)
    Mesh* crossQuadMesh;                // Pointer to the Mesh object (used for rendering CrossQuad objects, e.g. grass)
    bool isModified;                    // true if the user changed blocks (useful for saving to disk)

    // Constructor for convenience
    explicit ChunkData(std::shared_ptr<Chunk> c = nullptr, Mesh* om = nullptr, Mesh* tm = nullptr, Mesh* cqm = nullptr)
        : chunk(std::move(c)), opaqueMesh(om), transparentMesh(tm), crossQuadMesh(cqm), isModified(false) { }
};

#endif
