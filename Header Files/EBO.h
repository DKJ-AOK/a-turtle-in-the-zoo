#ifndef A_TURTLE_IN_THE_ZOO_EBO_H
#define A_TURTLE_IN_THE_ZOO_EBO_H

#include <vector>

class EBO {
    public:
        unsigned int ID{};
    EBO(const std::vector<unsigned int> &indices);

    void Bind() const;
    void Unbind() const;
    void Delete() const;
};

#endif //A_TURTLE_IN_THE_ZOO_EBO_H