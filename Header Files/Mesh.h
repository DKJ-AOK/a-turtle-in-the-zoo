#ifndef A_TURTLE_IN_THE_ZOO_MESH_H
#define A_TURTLE_IN_THE_ZOO_MESH_H

#include<string>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"

class Mesh {
public:
    std::vector <Vertex> vertices;
    std::vector <GLuint> indices;
    std::vector <Texture> textures;

    VAO VAO;

    Mesh(const std::vector <Vertex>& vertices, const std::vector <GLuint>& indices, const std::vector<Texture>& textures);

    void Draw(Shader &shader, Camera &camera);
};

#endif //A_TURTLE_IN_THE_ZOO_MESH_H