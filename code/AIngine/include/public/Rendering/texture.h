#pragma once

#include "Rendering/bitmap.h"

#include <glad/glad.h>
#include <string>

namespace AIngine::Rendering {

	class Texture2D {
	public:
		// Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
		GLuint ID = -1;

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
		Texture2D();
		// Creates the texture on the graphics card
		Texture2D(Bitmap &texture);

		// copy constructor
		Texture2D(const Texture2D& other);

		// copy assignment
		Texture2D& operator=(const Texture2D& other);

		// move constructor
		Texture2D(Texture2D&& other);

		// move assignment
		Texture2D& operator=(Texture2D&& other);

		virtual ~Texture2D();

		// Generates texture from a bitmap
		void Generate(Bitmap &texture);

		// Binds the texture as the current active GL_TEXTURE_2D texture object
		void Bind() const;

		std::string FileName;

	//	inline glm::vec3 GetColor() const { return m_color; }
	//	inline void SetColor(const glm::vec3& color) { m_color = color; }

	//	inline GLfloat GetAlpha() const { return m_alpha; }
	//	inline void SetAlpha(GLfloat alpha) { m_alpha = alpha; }

	//private:
	//	glm::vec3 m_color;
	//	GLfloat m_alpha = 1.0f;
	};
}

