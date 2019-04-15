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


/********************************************************************************************/

Texture2D::Texture2D()
	: Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
	glGenTextures(1, &this->ID);
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &this->ID);
}

void Texture2D::Generate(Bitmap &texture)
{
	this->Width = texture.GetWidth();
	this->Height = texture.GetHeight();
	// Create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, Width, Height, 0, this->Image_Format, GL_UNSIGNED_BYTE, texture.GetData());
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}