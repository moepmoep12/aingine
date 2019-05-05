#include "Rendering/texture.h"

namespace AIngine::Rendering {
	Texture2D::Texture2D(GameObject* owner)
		: Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR), m_spriteColor(glm::vec3(1.0)), Component(owner)
	{
		glGenTextures(1, &this->ID);
	}

	Texture2D::~Texture2D() 
	{
		glDeleteTextures(1, &this->ID);
	}

	void Texture2D::Generate(Bitmap &texture)
	{
		this->Width = texture.GetWidth();
		this->Height = texture.GetHeight();
		this->Image_Format = (texture.GetChannels() == 3) ? GL_RGB : GL_RGBA;
		this->Internal_Format = (texture.GetChannels() == 3) ? GL_RGB : GL_RGBA;
		// Create Texture
		glBindTexture(GL_TEXTURE_2D, this->ID);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			this->Internal_Format,
			Width,
			Height,
			0,
			this->Image_Format,
			GL_UNSIGNED_BYTE,
			texture.GetData());

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
}