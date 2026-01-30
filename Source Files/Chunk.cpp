#include "../Header Files/Chunk.h"

Chunk::Chunk(const glm::ivec3 pos, const std::uint32_t seed) : position(pos) {
    for (int x = 0; x < SIZE_X_Z; x++) {
        for (int z = 0; z < SIZE_X_Z; z++) {
            // Calculate world coordinates
            const auto worldX = static_cast<double>(pos.x * SIZE_X_Z + x);
            const auto worldZ = static_cast<double>(pos.z * SIZE_X_Z + z);

            const auto biome = getBiomeAtWorldPosition(glm::ivec2(worldX, worldZ), seed);

            // Map to height
            const auto height = getNoiseHeightAtWorldPosition(glm::ivec2(worldX, worldZ), seed, biome);

            for (int y = 0; y < SIZE_Y; y++) {
                if (y < height - 10) {
                    blocks[x][y][z] = STONE;
                }
                if (biome == PLANES || biome == SNOWY_TAIGA) {
                    if (y < height) {
                        blocks[x][y][z] = DIRT;
                    } else if (y == height) {
                        blocks[x][y][z] = biome == PLANES ? GRASS : SNOWY_GRASS;
                    }
                }
                else if (biome == DESSERT && y <= height) {
                    blocks[x][y][z] = SAND;
                }
                else {
                    blocks[x][y][z] = AIR;
                }
            }
        }
    }
}

int Chunk::getNoiseHeightAtWorldPosition(glm::ivec2 pos, uint32_t seed, Biome biome) {
    // Initialize the Perlin Noise object with a seed
    static const siv::PerlinNoise perlin{ seed };

    float frequency;     // Lower values = smoother hills
    int octaves;         // More octaves = more detail/jaggedness
    int surfaceHeight = 20;   // Base height

    switch (biome) {
        case PLANES:
        case DESSERT:
        case SNOWY_TAIGA:
            frequency = 0.025f;
            octaves = 4;
            break;
        default:
            frequency = 0.0f;
            octaves = 1;
            surfaceHeight = 0;
            break;
    }

    // Generate noise value between 0.0 and 1.0
    // octave2D_01 handles multiple layers of noise for you
    const double noise = perlin.octave2D_01(pos.x * frequency, pos.y * frequency, octaves);

    // Map to height
    return surfaceHeight + static_cast<int>(noise * surfaceHeight);
}

Biome Chunk::getBiomeAtWorldPosition(glm::ivec2 pos, std::uint32_t seed) {
    constexpr float frequency = 0.002f;     // Lower values = smoother hills
    constexpr int octaves = 4;             // More octaves = more detail/jaggedness

    std::mt19937 rng{seed};
    const uint32_t humiditySeed = rng();
    const uint32_t temperatureSeed = rng();

    static const siv::PerlinNoise humidityPerlin{ humiditySeed };
    static const siv::PerlinNoise temperaturePerlin{ temperatureSeed };

    const double humidity = humidityPerlin.octave2D_01(pos.x * frequency, pos.y * frequency, octaves);
    const double temperature = temperaturePerlin.octave2D_01(pos.x * frequency, pos.y * frequency, octaves);

    if (humidity <= 0.3f && temperature >= 0.7f) {
        return DESSERT;
    }
    if (humidity >= 0.7f && temperature <= 0.2f) {
        return SNOWY_TAIGA;
    }

    return PLANES;
}

void Chunk::addFace(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const glm::vec3 pos, const int faceDir, const BlockType type) {
    constexpr float size = 0.4f; // Based on main.cpp model translation
    const glm::vec3 p = pos * size;
    constexpr float s = size / 2.0f;
    
    UVRect uv = getUVs(type, faceDir);

    GLuint startIndex = vertices.size();

    if (faceDir == 0) { // Top
        vertices.push_back({p + glm::vec3(-s,  s,  s), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uv.uStart, uv.vStart)});
        vertices.push_back({p + glm::vec3(-s,  s, -s), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uv.uStart, uv.vEnd)});
        vertices.push_back({p + glm::vec3( s,  s, -s), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vEnd)});
        vertices.push_back({p + glm::vec3( s,  s,  s), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vStart)});
    } else if (faceDir == 1) { // Bottom
        vertices.push_back({p + glm::vec3(-s, -s, -s), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uv.uStart, uv.vStart)});
        vertices.push_back({p + glm::vec3(-s, -s,  s), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uv.uStart, uv.vEnd)});
        vertices.push_back({p + glm::vec3( s, -s,  s), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vEnd)});
        vertices.push_back({p + glm::vec3( s, -s, -s), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vStart)});
    } else if (faceDir == 2) { // Front
        vertices.push_back({p + glm::vec3(-s, -s,  s), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uv.uStart, uv.vStart)});
        vertices.push_back({p + glm::vec3( s, -s,  s), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vStart)});
        vertices.push_back({p + glm::vec3( s,  s,  s), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vEnd)});
        vertices.push_back({p + glm::vec3(-s,  s,  s), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uv.uStart, uv.vEnd)});
    } else if (faceDir == 3) { // Back
        vertices.push_back({p + glm::vec3( s, -s, -s), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uv.uStart, uv.vStart)});
        vertices.push_back({p + glm::vec3(-s, -s, -s), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vStart)});
        vertices.push_back({p + glm::vec3(-s,  s, -s), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vEnd)});
        vertices.push_back({p + glm::vec3( s,  s, -s), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uv.uStart, uv.vEnd)});
    } else if (faceDir == 4) { // Left
        vertices.push_back({p + glm::vec3(-s, -s, -s), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uv.uStart, uv.vStart)});
        vertices.push_back({p + glm::vec3(-s, -s,  s), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vStart)});
        vertices.push_back({p + glm::vec3(-s,  s,  s), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vEnd)});
        vertices.push_back({p + glm::vec3(-s,  s, -s), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uv.uStart, uv.vEnd)});
    } else if (faceDir == 5) { // Right
        vertices.push_back({p + glm::vec3( s, -s,  s), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uv.uStart, uv.vStart)});
        vertices.push_back({p + glm::vec3( s, -s, -s), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vStart)});
        vertices.push_back({p + glm::vec3( s,  s, -s), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uv.uEnd,   uv.vEnd)});
        vertices.push_back({p + glm::vec3( s,  s,  s), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uv.uStart, uv.vEnd)});
    }

    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);
}

UVRect Chunk::getUVs(BlockType type, int faceDir) {
    if (type == GRASS) {
        if (faceDir == 0) return getUVsForCoordinates(0, 0); // Top
        if (faceDir == 1) return getUVsForCoordinates(2, 0); // Bottom
        return getUVsForCoordinates(1, 0); // Sides
    } if (type == SNOWY_GRASS) {
        if (faceDir == 0) return getUVsForCoordinates(5, 0); // Top
        if (faceDir == 1) return getUVsForCoordinates(7, 0); // Bottom
        return getUVsForCoordinates(6, 0); // Sides
    }
    if (type == DIRT) return  getUVsForCoordinates(2, 0);
    if (type == STONE) return  getUVsForCoordinates(3, 0);
    if (type == SAND) return  getUVsForCoordinates(4, 0);
    return getUVsForCoordinates(15, 15);
}

MeshData* Chunk::generateMesh() const {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (int x = 0; x < SIZE_X_Z; x++) {
        for (int y = 0; y < SIZE_Y; y++) {
            for (int z = 0; z < SIZE_X_Z; z++) {
                if (blocks[x][y][z] == AIR) continue;

                glm::vec3 pos = glm::vec3(x, y, z) + glm::vec3(position * SIZE_X_Z);
                BlockType type = blocks[x][y][z];

                // Check neighbors
                // Top
                if (y == SIZE_X_Z - 1 || blocks[x][y+1][z] == AIR) addFace(vertices, indices, pos, 0, type);
                // Bottom
                if (y == 0 || blocks[x][y-1][z] == AIR) addFace(vertices, indices, pos, 1, type);
                // Front
                if (z == SIZE_X_Z - 1 || blocks[x][y][z+1] == AIR) addFace(vertices, indices, pos, 2, type);
                // Back
                if (z == 0 || blocks[x][y][z-1] == AIR) addFace(vertices, indices, pos, 3, type);
                // Left
                if (x == 0 || blocks[x-1][y][z] == AIR) addFace(vertices, indices, pos, 4, type);
                // Right
                if (x == SIZE_X_Z - 1 || blocks[x+1][y][z] == AIR) addFace(vertices, indices, pos, 5, type);
            }
        }
    }

    return new MeshData(vertices, indices);
}

void Chunk::addBlockAtWorldPosition(const glm::ivec3 pos, const BlockType type) {
    blocks[pos.x % SIZE_X_Z][pos.y][pos.z % SIZE_X_Z] = type;
}

BlockType Chunk::getBlockTypeAtWorldPosition(const glm::ivec3 pos) const {
    return blocks[pos.x % SIZE_X_Z][pos.y][pos.z % SIZE_X_Z];
}

UVRect Chunk::getUVsForCoordinates(int column, int row){
    float size = 1.0f / 16.0f;
    return UVRect{(column + 1) * size, column * size, 1 -(row + 1) * size,  1 - row * size};
}