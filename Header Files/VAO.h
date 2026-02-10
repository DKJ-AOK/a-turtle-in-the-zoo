#ifndef A_TURTLE_IN_THE_ZOO_VAO_H
#define A_TURTLE_IN_THE_ZOO_VAO_H

#include "../Header Files/VBO.h"

class VAO {
    public:
        unsigned int ID{};
    VAO();

    void LinkAttrib(VBO& VBO, unsigned int layout, unsigned int numComponents, unsigned int type, long long stride, void* offset) const;
    void Bind() const;
    void Unbind() const;
    void Delete() const;
};

#endif //A_TURTLE_IN_THE_ZOO_VAO_H