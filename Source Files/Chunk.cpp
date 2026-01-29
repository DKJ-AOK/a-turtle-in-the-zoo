#include "../Header Files/Chunk.h"

Chunk::Chunk(glm::ivec3 pos, std::uint32_t seed) : position(pos) {
    // Initialize the Perlin Noise object with a seed
    static const siv::PerlinNoise perlin{ seed };

    float frequency = 0.025f; // Lower values = smoother hills
    int octaves = 4;         // More octaves = more detail/jaggedness
    int surfaceHeight = 20;   // Base height

    for (int x = 0; x < SIZE_X_Z; x++) {
        for (int z = 0; z < SIZE_X_Z; z++) {
            // Calculate world coordinates
            auto worldX = static_cast<double>(pos.x * SIZE_X_Z + x);
            auto worldZ = static_cast<double>(pos.z * SIZE_X_Z + z);

            // Generate noise value between 0.0 and 1.0
            // octave2D_01 handles multiple layers of noise for you
            double noise = perlin.octave2D_01(worldX * frequency, worldZ * frequency, octaves);

            // Map to height (e.g., base height + up to 8 blocks)
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

void Chunk::addFace(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, glm::vec3 pos, int faceDir, BlockType type) {
    float size = 0.4f; // Based on main.cpp model translation
    glm::vec3 p = pos * size;
    float s = size / 2.0f;

    // UV offsets based on texture sheet
    // 0.0 - 0.25: Grass Top
    // 0.25 - 0.5: Grass Side (Dirt with Grass)
    // 0.5 - 0.75: Dirt
    // 0.75 - 1.0: Stone (Last 4th as requested)
    
    float uStart = 0.0f;
    float uEnd = 0.25f;

    if (type == STONE) {
        uStart = 0.75f;
        uEnd = 1.0f;
    } else if (type == GRASS) {
        if (faceDir == 0) { // Top
            uStart = 0.0f; uEnd = 0.25f;
        } else if (faceDir == 1) { // Bottom
            uStart = 0.5f; uEnd = 0.75f;
        } else { // Sides
            uStart = 0.25f; uEnd = 0.5f;
        }
    } else if (type == DIRT) {
        uStart = 0.5f; uEnd = 0.75f;
    }

    GLuint startIndex = vertices.size();

    if (faceDir == 0) { // Top
        vertices.push_back({p + glm::vec3(-s,  s,  s), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uStart, 0)});
        vertices.push_back({p + glm::vec3(-s,  s, -s), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uStart, 1)});
        vertices.push_back({p + glm::vec3( s,  s, -s), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uEnd, 1)});
        vertices.push_back({p + glm::vec3( s,  s,  s), glm::vec3(0, 1, 0), glm::vec3(1), glm::vec2(uEnd, 0)});
    } else if (faceDir == 1) { // Bottom
        vertices.push_back({p + glm::vec3(-s, -s, -s), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uStart, 0)});
        vertices.push_back({p + glm::vec3(-s, -s,  s), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uStart, 1)});
        vertices.push_back({p + glm::vec3( s, -s,  s), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uEnd, 1)});
        vertices.push_back({p + glm::vec3( s, -s, -s), glm::vec3(0, -1, 0), glm::vec3(1), glm::vec2(uEnd, 0)});
    } else if (faceDir == 2) { // Front
        vertices.push_back({p + glm::vec3(-s, -s,  s), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uStart, 0)});
        vertices.push_back({p + glm::vec3( s, -s,  s), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uEnd, 0)});
        vertices.push_back({p + glm::vec3( s,  s,  s), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uEnd, 1)});
        vertices.push_back({p + glm::vec3(-s,  s,  s), glm::vec3(0, 0, 1), glm::vec3(1), glm::vec2(uStart, 1)});
    } else if (faceDir == 3) { // Back
        vertices.push_back({p + glm::vec3( s, -s, -s), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uStart, 0)});
        vertices.push_back({p + glm::vec3(-s, -s, -s), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uEnd, 0)});
        vertices.push_back({p + glm::vec3(-s,  s, -s), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uEnd, 1)});
        vertices.push_back({p + glm::vec3( s,  s, -s), glm::vec3(0, 0, -1), glm::vec3(1), glm::vec2(uStart, 1)});
    } else if (faceDir == 4) { // Left
        vertices.push_back({p + glm::vec3(-s, -s, -s), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uStart, 0)});
        vertices.push_back({p + glm::vec3(-s, -s,  s), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uEnd, 0)});
        vertices.push_back({p + glm::vec3(-s,  s,  s), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uEnd, 1)});
        vertices.push_back({p + glm::vec3(-s,  s, -s), glm::vec3(-1, 0, 0), glm::vec3(1), glm::vec2(uStart, 1)});
    } else if (faceDir == 5) { // Right
        vertices.push_back({p + glm::vec3( s, -s,  s), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uStart, 0)});
        vertices.push_back({p + glm::vec3( s, -s, -s), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uEnd, 0)});
        vertices.push_back({p + glm::vec3( s,  s, -s), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uEnd, 1)});
        vertices.push_back({p + glm::vec3( s,  s,  s), glm::vec3(1, 0, 0), glm::vec3(1), glm::vec2(uStart, 1)});
    }

    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);
}

Mesh* Chunk::generateMesh(std::vector<Texture>& textures) {
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
