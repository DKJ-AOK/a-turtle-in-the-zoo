#include "../Header Files/Chunk.h"

Chunk::Chunk(const glm::ivec3 pos, const std::uint32_t seed) : position(pos) {
    for (int x = 0; x < SIZE_X_Z; x++) {
        for (int z = 0; z < SIZE_X_Z; z++) {
            // Calculate world coordinates
            const auto worldX = static_cast<double>(pos.x * SIZE_X_Z + x);
            const auto worldZ = static_cast<double>(pos.z * SIZE_X_Z + z);

            const auto weights = getBiomeWeightsAtWorldPosition(glm::ivec2(worldX, worldZ), seed);

            // Map to height
            const auto height = getNoiseHeightAtWorldPosition(glm::ivec2(worldX, worldZ), seed, weights);

            // Determine primary biome for block selection
            BlockType surfaceBlock = GRASS;
            BlockType subSurfaceBlock = DIRT;

            if (weights.desert > weights.plains && weights.desert > weights.mountain && weights.desert > weights.snowyTaiga) {
                surfaceBlock = SAND;
                subSurfaceBlock = SAND;
            } else if (weights.snowyTaiga > weights.plains && weights.snowyTaiga > weights.mountain && weights.snowyTaiga > weights.desert) {
                surfaceBlock = SNOWY_GRASS;
                subSurfaceBlock = DIRT;
            } else if (weights.mountain > weights.plains && weights.mountain > weights.desert && weights.mountain > weights.snowyTaiga) {
                surfaceBlock = STONE;
                subSurfaceBlock = STONE;
            }

            for (int y = 0; y < SIZE_Y; y++) {
                if (y < height - 4) {
                    blocks[x][y][z] = STONE;
                } else if (y < height) {
                    blocks[x][y][z] = subSurfaceBlock;
                } else if (y == height) {
                    blocks[x][y][z] = surfaceBlock;
                } else {
                    blocks[x][y][z] = AIR;
                }
            }
        }
    }
}

int Chunk::getNoiseHeightAtWorldPosition(glm::ivec2 pos, uint32_t seed, const BiomeWeights& weights) const {
    static const siv::PerlinNoise perlin{ seed };

    double hPlains = 0, hMountain = 0, hDesert = 0, hSnowyTaiga = 0;

    if (weights.plains > 0)
        hPlains = plains.baseHeight + perlin.octave2D_01(pos.x * plains.frequency, pos.y * plains.frequency, plains.octaves) * plains.baseHeight;
    if (weights.mountain > 0)
        hMountain = mountain.baseHeight + perlin.octave2D_01(pos.x * mountain.frequency, pos.y * mountain.frequency, mountain.octaves) * mountain.baseHeight;
    if (weights.desert > 0)
        hDesert = desert.baseHeight + perlin.octave2D_01(pos.x * desert.frequency, pos.y * desert.frequency, desert.octaves) * desert.baseHeight;
    if (weights.snowyTaiga > 0)
        hSnowyTaiga = snowyTaiga.baseHeight + perlin.octave2D_01(pos.x * snowyTaiga.frequency, pos.y * snowyTaiga.frequency, snowyTaiga.octaves) * snowyTaiga.baseHeight;

    float finalHeight = (hPlains * weights.plains) + (hMountain * weights.mountain) + (hDesert * weights.desert) + (hSnowyTaiga * weights.snowyTaiga);

    return static_cast<int>(finalHeight);
}

BiomeWeights Chunk::getBiomeWeightsAtWorldPosition(glm::ivec2 pos, std::uint32_t seed) {
    constexpr float frequency = 0.002f;
    constexpr int octaves = 4;

    std::mt19937 rng{seed};
    const uint32_t humiditySeed = rng();
    const uint32_t temperatureSeed = rng();

    static const siv::PerlinNoise humidityPerlin{ humiditySeed };
    static const siv::PerlinNoise temperaturePerlin{ temperatureSeed };

    const double humidity = humidityPerlin.octave2D_01(pos.x * frequency, pos.y * frequency, octaves);
    const double temperature = temperaturePerlin.octave2D_01(pos.x * frequency, pos.y * frequency, octaves);

    auto smoothstep = [](float edge0, float edge1, float x) {
        float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    };

    // Define biome regions and calculate weights
    // Desert: Low humidity, high temperature
    float wDesert = (1.0f - smoothstep(0.3f, 0.5f, humidity)) * smoothstep(0.5f, 0.7f, temperature);

    // Snowy Taiga: High humidity, low temperature
    float wSnowyTaiga = smoothstep(0.5f, 0.7f, humidity) * (1.0f - smoothstep(0.2f, 0.4f, temperature));
    
    // Mountains: Low humidity, medium/low temperature
    float wMountain = smoothstep(0.4f, 0.6f, humidity) * (1.0f - smoothstep(0.3f, 0.5f, temperature));

    // Plains: Default biome, fill the rest
    float wPlains = 1.0f - std::max({wDesert, wSnowyTaiga, wMountain});
    wPlains = std::max(0.0f, wPlains);

    // Normalize weights so they sum to 1
    float total = wDesert + wSnowyTaiga + wMountain + wPlains;
    return { wPlains / total, wMountain / total, wDesert / total, wSnowyTaiga / total };
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