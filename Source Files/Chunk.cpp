#include "../Header Files/Chunk.h"
#include "../Header Files/World.h"

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
                if (y > height && y <= seaLevel) {
                    blocks[x][y][z] = WATER;
                } else if (y < height - 4) {
                    blocks[x][y][z] = STONE;
                } else if (y < height) {
                    blocks[x][y][z] = subSurfaceBlock;
                } else if (y == height) {
                    blocks[x][y][z] = surfaceBlock;
                } else if (y == height + 1 && height >= seaLevel && (surfaceBlock == GRASS || surfaceBlock == SNOWY_GRASS)) {
                    // Use a simple deterministic hash to decide where to spawn decorations
                    const uint32_t decorationHash = (static_cast<uint32_t>(worldX) * 73856093) ^
                                                     (static_cast<uint32_t>(worldZ) * 19349663) ^ seed;

                    if (decorationHash % 15 == 0) { // ~6.6% chance for grass
                        blocks[x][y][z] = GRASS_QUAD;
                    } else if (decorationHash % 100 == 0) { // 1% chance for flowers
                        blocks[x][y][z] = RED_FLOWER_QUAD;
                    } else {
                        blocks[x][y][z] = AIR;
                    }
                } else {
                    blocks[x][y][z] = AIR;
                }
            }
        }
    }
}

int Chunk::getNoiseHeightAtWorldPosition(const glm::ivec2 pos, const uint32_t seed, const BiomeWeights& weights) const {
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

BiomeWeights Chunk::getBiomeWeightsAtWorldPosition(const glm::ivec2 pos, const std::uint32_t seed) {
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

void Chunk::addCrossQuadFaces(ShapeData &shapes, const glm::vec3 pos) const {
    const int x = static_cast<int>(pos.x);
    const int y = static_cast<int>(pos.y);
    const int z = static_cast<int>(pos.z);

    // 1. Bounds check and validation
    if (y <= 0 || y >= SIZE_Y) return;

    // Only allow placement on top of grass/snowy grass (optional but common for plants)
    if (const auto blockBelow = blocks[x][y - 1][z];
        blockBelow != GRASS && blockBelow != SNOWY_GRASS) {
        return;
    }

    const BlockType type = blocks[x][y][z];
    const glm::ivec3 worldPos = glm::ivec3(x, y, z) + position * SIZE_X_Z;
    const glm::vec3 blockPos = glm::vec3(worldPos) * BLOCK_SCALE;
    constexpr float size = BLOCK_SCALE / 2.0f;

    // 2. Get UVs for the block type
    // Note: You must update Chunk::getUVs to handle GRASS_QUAD and RED_FLOWER_QUAD
    auto [uStart, uEnd, vStart, vEnd] = getUVs(type, 0);

    // 3. Diagonal 1 (Back-Left to Front-Right)
    GLuint startIdx = shapes.vertices.size();
    // Normal is often set to (0, 1, 0) for consistent lighting on plants
    constexpr glm::vec3 normal(0, 1, 0);

    shapes.vertices.push_back({blockPos + glm::vec3(-size, -size, -size), normal, glm::vec3(1), glm::vec2(uStart, vStart)});
    shapes.vertices.push_back({blockPos + glm::vec3( size, -size,  size), normal, glm::vec3(1), glm::vec2(uEnd,   vStart)});
    shapes.vertices.push_back({blockPos + glm::vec3( size,  size,  size), normal, glm::vec3(1), glm::vec2(uEnd,   vEnd)});
    shapes.vertices.push_back({blockPos + glm::vec3(-size,  size, -size), normal, glm::vec3(1), glm::vec2(uStart, vEnd)});

    shapes.indices.push_back(startIdx + 0);
    shapes.indices.push_back(startIdx + 1);
    shapes.indices.push_back(startIdx + 2);
    shapes.indices.push_back(startIdx + 0);
    shapes.indices.push_back(startIdx + 2);
    shapes.indices.push_back(startIdx + 3);

    // 4. Diagonal 2 (Front-Left to Back-Right)
    startIdx = shapes.vertices.size();
    shapes.vertices.push_back({blockPos + glm::vec3(-size, -size,  size), normal, glm::vec3(1), glm::vec2(uStart, vStart)});
    shapes.vertices.push_back({blockPos + glm::vec3( size, -size, -size), normal, glm::vec3(1), glm::vec2(uEnd,   vStart)});
    shapes.vertices.push_back({blockPos + glm::vec3( size,  size, -size), normal, glm::vec3(1), glm::vec2(uEnd,   vEnd)});
    shapes.vertices.push_back({blockPos + glm::vec3(-size,  size,  size), normal, glm::vec3(1), glm::vec2(uStart, vEnd)});

    shapes.indices.push_back(startIdx + 0);
    shapes.indices.push_back(startIdx + 1);
    shapes.indices.push_back(startIdx + 2);
    shapes.indices.push_back(startIdx + 0);
    shapes.indices.push_back(startIdx + 2);
    shapes.indices.push_back(startIdx + 3);
}

void Chunk::addBlockFaces(World& world, ShapeData& shapes, const glm::vec3 pos, const BlockType blockType) const {
    // Convert chunk relative position to world position
    const glm::ivec3 worldPos = glm::ivec3(pos.x, pos.y, pos.z) + position * SIZE_X_Z;

    // Lambda for checking neighbor visibility
    auto checkNeighbor = [&](const int nx, const int ny, const int nz, const glm::ivec3& dir) {
        // If neighbor is out of bounds, check visibility in world
        if (nx < 0 || nx >= SIZE_X_Z || ny < 0 || ny >= SIZE_Y || nz < 0 || nz >= SIZE_X_Z) {
            return world.isFaceVisible(worldPos, dir, blockType);
        }

        // Get neighbor block type
        const BlockType neighborBlock = blocks[nx][ny][nz];
        if (blockType == neighborBlock && blockType == WATER) return false;
        if (std::ranges::any_of(NO_COLLISION_BLOCKS, [&](const BlockType quadBlock)
            { return quadBlock == neighborBlock; }))
            return true;

        return false;
    };

    // Top
    if (checkNeighbor(pos.x, pos.y + 1, pos.z, {0, 1, 0}))
        addBlockFace(shapes.vertices, shapes.indices,
            worldPos, 0, blockType);
    // Bottom
    if (checkNeighbor(pos.x, pos.y - 1, pos.z, {0, -1, 0}))
        addBlockFace(shapes.vertices, shapes.indices,
            worldPos, 1, blockType);
    // Front
    if (checkNeighbor(pos.x, pos.y, pos.z + 1, {0, 0, 1}))
        addBlockFace(shapes.vertices, shapes.indices,
            worldPos, 2, blockType);
    // Back
    if (checkNeighbor(pos.x, pos.y, pos.z - 1, {0, 0, -1}))
        addBlockFace(shapes.vertices, shapes.indices,
            worldPos, 3, blockType);
    // Left
    if (checkNeighbor(pos.x - 1, pos.y, pos.z, {-1, 0, 0}))
        addBlockFace(shapes.vertices, shapes.indices, worldPos, 4, blockType);
    // Right
    if (checkNeighbor(pos.x + 1, pos.y, pos.z, {1, 0, 0}))
        addBlockFace(shapes.vertices, shapes.indices, worldPos, 5, blockType);
}

void Chunk::addBlockFace(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const glm::vec3 pos, const int faceDir, const BlockType blockType) {
    const glm::vec3 blockPos = pos * BLOCK_SCALE;
    constexpr float size = BLOCK_SCALE / 2.0f;

    auto [uStart, uEnd, vStart, vEnd] = getUVs(blockType, faceDir);

    const GLuint startIndex = vertices.size();

    if (faceDir == 0) { // Top
        vertices.push_back({blockPos + glm::vec3(-size,  size,  size), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uStart, vStart)});
        vertices.push_back({blockPos + glm::vec3( size,  size,  size), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uEnd,   vStart)});
        vertices.push_back({blockPos + glm::vec3( size,  size, -size), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uEnd,   vEnd)});
        vertices.push_back({blockPos + glm::vec3(-size,  size, -size), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uStart, vEnd)});
    } else if (faceDir == 1) { // Bottom
        vertices.push_back({blockPos + glm::vec3(-size, -size, -size), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uStart, vStart)});
        vertices.push_back({blockPos + glm::vec3( size, -size, -size), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uEnd,   vStart)});
        vertices.push_back({blockPos + glm::vec3( size, -size,  size), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uEnd,   vEnd)});
        vertices.push_back({blockPos + glm::vec3(-size, -size,  size), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uStart, vEnd)});
    } else if (faceDir == 2) { // Front
        vertices.push_back({blockPos + glm::vec3(-size, -size,  size), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uStart, vStart)});
        vertices.push_back({blockPos + glm::vec3( size, -size,  size), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uEnd,   vStart)});
        vertices.push_back({blockPos + glm::vec3( size,  size,  size), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uEnd,   vEnd)});
        vertices.push_back({blockPos + glm::vec3(-size,  size,  size), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uStart, vEnd)});
    } else if (faceDir == 3) { // Back
        vertices.push_back({blockPos + glm::vec3( size, -size, -size), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uStart, vStart)});
        vertices.push_back({blockPos + glm::vec3(-size, -size, -size), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uEnd,   vStart)});
        vertices.push_back({blockPos + glm::vec3(-size,  size, -size), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uEnd,   vEnd)});
        vertices.push_back({blockPos + glm::vec3( size,  size, -size), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uStart, vEnd)});
    } else if (faceDir == 4) { // Left
        vertices.push_back({blockPos + glm::vec3(-size, -size, -size), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uStart, vStart)});
        vertices.push_back({blockPos + glm::vec3(-size, -size,  size), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uEnd,   vStart)});
        vertices.push_back({blockPos + glm::vec3(-size,  size,  size), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uEnd,   vEnd)});
        vertices.push_back({blockPos + glm::vec3(-size,  size, -size), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uStart, vEnd)});
    } else if (faceDir == 5) { // Right
        vertices.push_back({blockPos + glm::vec3( size, -size,  size), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uStart, vStart)});
        vertices.push_back({blockPos + glm::vec3( size, -size, -size), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uEnd,   vStart)});
        vertices.push_back({blockPos + glm::vec3( size,  size, -size), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uEnd,   vEnd)});
        vertices.push_back({blockPos + glm::vec3( size,  size,  size), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uStart, vEnd)});
    }

    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);
}

UVRect Chunk::getUVs(const BlockType type, const int faceDir = 0) {
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
    if (type == WATER) return getUVsForCoordinates(0, 1);
    if (type == GRASS_QUAD) return getUVsForCoordinates(0, 3);
    if (type == RED_FLOWER_QUAD) return getUVsForCoordinates(1, 3);
    return getUVsForCoordinates(15, 15);
}

MeshData* Chunk::generateMesh(World& world) const {
    ShapeData opaqueShapes;
    ShapeData transparentShapes;
    ShapeData crossQuadShapes;

    for (int x = 0; x < SIZE_X_Z; x++) {
        for (int y = 0; y < SIZE_Y; y++) {
            for (int z = 0; z < SIZE_X_Z; z++) {
                const BlockType currentBlock = blocks[x][y][z];

                // If block is air, skip
                if (currentBlock == AIR) continue;

                if (currentBlock == WATER) {
                    addBlockFaces(world, transparentShapes, glm::vec3(x, y, z), currentBlock);
                } else if (currentBlock == GRASS_QUAD || currentBlock == RED_FLOWER_QUAD) {
                    addCrossQuadFaces(crossQuadShapes, {x, y, z});
                } else {
                    addBlockFaces(world, opaqueShapes, glm::vec3(x, y, z), currentBlock);
                }
            }
        }
    }

    return new MeshData{opaqueShapes, transparentShapes, crossQuadShapes };
}

void Chunk::addBlockAtWorldPosition(const glm::ivec3 pos, const BlockType type) {
    if (pos.y < 0 || pos.y >= SIZE_Y) return;
    const int x = ((pos.x % SIZE_X_Z) + SIZE_X_Z) % SIZE_X_Z;
    const int z = ((pos.z % SIZE_X_Z) + SIZE_X_Z) % SIZE_X_Z;
    blocks[x][pos.y][z] = type;
}

BlockType Chunk::getBlockTypeAtWorldPosition(const glm::ivec3 pos) const {
    if (pos.y < 0 || pos.y >= SIZE_Y) return AIR;
    const int x = ((pos.x % SIZE_X_Z) + SIZE_X_Z) % SIZE_X_Z;
    const int z = ((pos.z % SIZE_X_Z) + SIZE_X_Z) % SIZE_X_Z;
    return blocks[x][pos.y][z];
}

UVRect Chunk::getUVsForCoordinates(const int column, const int row){
    float size = 1.0f / 16.0f;
    return UVRect{(column + 1) * size, column * size, 1 -(row + 1) * size,  1 - row * size};
}