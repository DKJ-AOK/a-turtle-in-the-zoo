#include "../Header Files/Chunk.h"

Chunk::Chunk(const glm::ivec3 pos, const std::uint32_t seed) : position(pos) {
    // Initialize the Perlin Noise object with a seed
    static const siv::PerlinNoise perlin{ seed };

    const float frequency = 0.025f; // Lower values = smoother hills
    const int octaves = 4;         // More octaves = more detail/jaggedness
    const int surfaceHeight = 20;   // Base height

    for (int x = 0; x < SIZE_X_Z; x++) {
        for (int z = 0; z < SIZE_X_Z; z++) {
            // Calculate world coordinates
            auto worldX = static_cast<double>(pos.x * SIZE_X_Z + x);
            auto worldZ = static_cast<double>(pos.z * SIZE_X_Z + z);

            // Generate noise value between 0.0 and 1.0
            // octave2D_01 handles multiple layers of noise for you
            double noise = perlin.octave2D_01(worldX * frequency, worldZ * frequency, octaves);

            // Map to height
            auto height = surfaceHeight + static_cast<int>(noise * surfaceHeight);

            for (int y = 0; y < SIZE_Y; y++) {
                if (y < height - 10) {
                    blocks[x][y][z] = STONE;
                } else if (y < height) {
                    blocks[x][y][z] = DIRT;
                } else if (y == height) {
                    blocks[x][y][z] = GRASS;
                } else {
                    blocks[x][y][z] = AIR;
                }
            }
        }
    }
}

void Chunk::addFace(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const glm::vec3 pos, const int faceDir, const BlockType type) {
    float size = 0.4f; // Based on main.cpp model translation
    glm::vec3 p = pos * size;
    float s = size / 2.0f;
    
    UVRect uv = getUVs(15, 15);

    if (type == GRASS) {
        if (faceDir == 0) uv = getUVs(0, 0); // Top
        else if (faceDir == 1) uv = getUVs(2, 0); // Bottom
        else uv = getUVs(1, 0); // Sides
    }
    else if (type == DIRT) uv = getUVs(2, 0);
    else if (type == STONE) uv = getUVs(3, 0);
    else if (type == SAND) uv = getUVs(4, 0);

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

Mesh* Chunk::generateMesh(const std::vector<Texture>& textures) const {
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

    return new Mesh(vertices, indices, textures);
}

void Chunk::addBlockAtWorldPosition(const glm::ivec3 pos, const BlockType type) {
    blocks[pos.x % SIZE_X_Z][pos.y][pos.z % SIZE_X_Z] = type;
}

BlockType Chunk::getBlockTypeAtWorldPosition(const glm::ivec3 pos) const {
    return blocks[pos.x % SIZE_X_Z][pos.y][pos.z % SIZE_X_Z];
}

UVRect Chunk::getUVs(int column, int row){
    float size = 1.0f / 16.0f;
    return UVRect{(column + 1) * size, column * size, 1 -(row + 1) * size,  1 - row * size};
}