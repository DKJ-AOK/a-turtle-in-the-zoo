#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include"shaderClass.h"

class Texture
{
public:
    unsigned int ID;
    const char* type;
    unsigned int unit;

    Texture(const char* image, const char* texType, unsigned int slot, unsigned int format, unsigned int pixelType);

    // Assigns a texture unit to a texture
    void texUnit(const Shader& shader, const char* uniform, unsigned int unit);
    // Binds a texture
    void Bind();
    // Unbinds a texture
    void Unbind();
    // Deletes a texture
    void Delete();
};
#endif