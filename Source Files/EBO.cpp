#include "../Header Files/EBO.h"

#include <glad/gl.h>

EBO::EBO(const std::vector<GLuint> &indices) {
    // Generate the EBO with only 1 object each
    glGenBuffers(1, &ID);
    // Bind the EBO specifying it's a GL_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    // Introduce the vertices into the EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void EBO::Bind() const {
    // Bind the EBO specifying it's a GL_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete() const {
    glDeleteBuffers(1, &ID);
}
