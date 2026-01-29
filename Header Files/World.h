//
// Created by danie on 28/01/2026.
//

#ifndef A_TURTLE_IN_THE_ZOO_WORLD_H
#define A_TURTLE_IN_THE_ZOO_WORLD_H
#include <map>
#include <vector>
#include <glm/vec3.hpp>

#include "Chunk.h"

class World {
public:
    explicit World(std::uint32_t seed, const std::vector<Texture>& textures) : textures(textures), seed(seed) {};
    void updateChunks(glm::vec3 cameraPos);
    void draw(Shader& shader, Camera& camera) const;
    void updateRenderDistance(int newDistance);
    [[nodiscard]] int getRenderDistance() const { return renderDistance; }

    void addBlock(glm::ivec3 pos, BlockType type);
    void removeBlock(glm::ivec3 pos);

private:
    std::map<std::pair<int, int>, ChunkData> chunks;
    std::vector<Texture> textures;
    std::uint32_t seed;
    int renderDistance = 16;
};

#endif //A_TURTLE_IN_THE_ZOO_WORLD_H