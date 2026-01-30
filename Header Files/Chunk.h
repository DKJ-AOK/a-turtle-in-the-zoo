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
    SAND = 4,
    SNOWY_GRASS = 5,
    WATER = 6
};

enum Biome {
    PLANES = 0,
    DESSERT = 1,
    MOUNTAINS = 2, // TODO: Make this
    SNOWY_TAIGA = 3
};

struct UVRect {
    float uStart, uEnd, vStart, vEnd;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
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
    BlockType blocks[SIZE_X_Z][SIZE_Y][SIZE_X_Z]{};
    glm::ivec3 position;

    explicit Chunk(glm::ivec3 pos, std::uint32_t seed = 0);
    [[nodiscard]] MeshData* generateMesh() const;

    void addBlockAtWorldPosition(glm::ivec3 pos, BlockType type);
    [[nodiscard]] BlockType getBlockTypeAtWorldPosition(glm::ivec3 pos) const;

private:
    static void addFace(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, glm::vec3 pos, int faceDir, BlockType type);
    static UVRect getUVsForCoordinates(int column, int row);
    static UVRect getUVs(BlockType type, int faceDir);

    [[nodiscard]] int getNoiseHeightAtWorldPosition(glm::ivec2 pos, uint32_t seed, const BiomeWeights& weights) const;
    [[nodiscard]] static BiomeWeights getBiomeWeightsAtWorldPosition(glm::ivec2 pos, std::uint32_t seed);

    BiomeSettings plains   = { 0.025f, 4, 20 };
    BiomeSettings mountain = { 0.050f, 6, 60 };
    BiomeSettings desert   = { 0.020f, 3, 15 };
    BiomeSettings snowyTaiga = { 0.025f, 4, 25 };

    int seaLevel = 20;
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
