#ifndef A_TURTLE_IN_THE_ZOO_VBO_H
#define A_TURTLE_IN_THE_ZOO_VBO_H

#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texUV;
};

class VBO {
    public:
        unsigned int ID{};
    VBO(const std::vector<Vertex>& vertices);
    VBO(const std::vector<glm::mat4>& matrices);

    void Bind() const;
    void Unbind() const;
    void Delete() const;
};

#endif //A_TURTLE_IN_THE_ZOO_VBO_H