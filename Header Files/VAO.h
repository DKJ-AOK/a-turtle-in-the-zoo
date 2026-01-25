#ifndef A_TURTLE_IN_THE_ZOO_VAO_H
#define A_TURTLE_IN_THE_ZOO_VAO_H

#include <glad/glad.h>
#include "../Header Files/VBO.h"

class VAO {
    public:
        GLuint ID{};
    VAO();

    void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) const;
    void Bind() const;
    void Unbind() const;
    void Delete() const;
};

#endif //A_TURTLE_IN_THE_ZOO_VAO_H