#include "Rendering/texture.h"

Texture::Texture() {
    glGenTextures(1, &textureBuffer);
    glGenSamplers(1, &trilinearSampler);

    glSamplerParameteri(trilinearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(trilinearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureBuffer);
    glDeleteSamplers(1, &trilinearSampler);
}

void Texture::SetTexture(Bitmap &texture) {
    glBindTexture(GL_TEXTURE_2D, textureBuffer);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_SRGB8,
                 texture.GetWidth(),
                 texture.GetHeight(),
                 0,
                 (texture.GetChannels() == 3) ? GL_RGB : GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 texture.GetData());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(GLShaderProgram * sp) {
    const GLint colorTextureUniform = glGetUniformLocation(sp->GetID(), "colorTexture");

    if (colorTextureUniform != -1) {
        glActiveTexture(GL_TEXTURE0);
        glBindSampler(0, trilinearSampler);
        glBindTexture(GL_TEXTURE_2D, textureBuffer);
    }
}

void Texture::Unbind(GLShaderProgram *sp) {
    const GLint colorTextureUniform = glGetUniformLocation(sp->GetID(), "colorTexture");

    if (colorTextureUniform != -1) glDisableVertexAttribArray(colorTextureUniform);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindSampler(0, 0);
}
