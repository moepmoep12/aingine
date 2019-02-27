#pragma once

#include <glad/glad.h>

#include "shader.h"
#include "bitmap.h"

class Texture {
public:
    Texture();

    ~Texture();

    void SetTexture(Bitmap &texture);

    void Bind(GLShaderProgram *sp);

    void Unbind(GLShaderProgram *sp);

private:
    GLuint textureBuffer = GL_NONE;
    GLuint trilinearSampler = GL_NONE;
};
