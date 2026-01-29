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
    std::map<std::pair<int, int>, ChunkData> chunks;
    std::uint32_t seed;

    World(std::uint32_t seed);
    void updateChunks(glm::vec3 cameraPos, std::vector<Texture>& textures);
    void draw(Shader& shader, Camera& camera);
    int renderDistance = 8;
};

#endif //A_TURTLE_IN_THE_ZOO_WORLD_H