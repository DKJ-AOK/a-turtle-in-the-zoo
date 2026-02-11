#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <string>

std::string get_file_contents(const char* filename);

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexFile, const char* fragmentFile);

    void Activate() const;
    void Delete() const;
private:
    void compileErrors(unsigned int shader, const char* type);
};

#endif //SHADER_CLASS_H