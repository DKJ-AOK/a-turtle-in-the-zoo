#include "../Header Files/VAO.h"

VAO::VAO() {
    // Generate the VAO and VBO with only 1 object each
    glGenVertexArrays(1, &ID);
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void *offset) const {
    VBO.Bind();
    // Configure the Vertex Attribute so that OpenGL knows how to read the VBO
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    // Enable the Vertex Attribute so that OpenGL knows to use it
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
}

void VAO::Bind() const {
    // make the VAO the current Vertex Array Object by binding it
    glBindVertexArray(ID);
}

void VAO::Unbind() const {
    glBindVertexArray(0);
}

void VAO::Delete() const {
    glDeleteVertexArrays(1, &ID);
}
