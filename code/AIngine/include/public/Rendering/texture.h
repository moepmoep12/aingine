#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "bitmap.h"
#include "Component.h"

namespace AIngine::Rendering {

	class Texture2D : public AIngine::Component {
	public:
		// Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
		GLuint ID;

		// Texture image dimensions
		GLuint Width, Height; // Width and height of loaded image in pixels

		// Texture Format
		GLuint Internal_Format; // Format of texture object
		GLuint Image_Format; // Format of loaded image

		// Texture configuration
		GLuint Wrap_S; // Wrapping mode on S axis
		GLuint Wrap_T; // Wrapping mode on T axis
		GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
		GLuint Filter_Max; // Filtering mode if texture pixels > screen pixels

		// Constructor (sets default texture modes)
		Texture2D(GameObject* owner);
		virtual ~Texture2D();

		// Generates texture from a bitmap
		void Generate(Bitmap &texture);

		// Binds the texture as the current active GL_TEXTURE_2D texture object
		void Bind() const;

		inline const glm::vec3& GetColor() const { return m_spriteColor; }
		inline void SetColor(const glm::vec3& color) { m_spriteColor = color; }

		const glm::vec2& GetLocalSize() const;

	private:
		glm::vec3 m_spriteColor;

	};
}

