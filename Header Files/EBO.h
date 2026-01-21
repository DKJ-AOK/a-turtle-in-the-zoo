#ifndef A_TURTLE_IN_THE_ZOO_EBO_H
#define A_TURTLE_IN_THE_ZOO_EBO_H

#include <glad/glad.h>

class EBO {
    public:
        GLuint ID{};
    EBO(const GLuint* indices, GLsizeiptr size);

    void Bind() const;
    void Unbind() const;
    void Delete() const;
};

#endif //A_TURTLE_IN_THE_ZOO_EBO_H