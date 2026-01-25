#ifndef A_TURTLE_IN_THE_ZOO_VBO_H
#define A_TURTLE_IN_THE_ZOO_VBO_H

#include <glad/gl.h>

class VBO {
    public:
        GLuint ID{};
    VBO(const GLfloat* vertices, GLsizeiptr size);

    void Bind() const;
    void Unbind() const;
    void Delete() const;
};

#endif //A_TURTLE_IN_THE_ZOO_VBO_H