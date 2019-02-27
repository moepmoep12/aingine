#pragma once

#include "shader.h"
#include <glad/glad.h>

namespace AIngine::Renderer {
	class Mesh {
	public:
		Mesh();

		~Mesh();

		void Bind(GLShaderProgram *sp);

		void Draw();

		void SetPositions(float *positions, size_t count);

		void SetTexCoordinates(float *uvs, size_t count);

		void SetIndices(unsigned int *indices, size_t count);

	public:
		GLuint positionBuffer = GL_NONE;
		GLuint texCoordBuffer = GL_NONE;
		GLuint indexBuffer = GL_NONE;

		GLint positionAttribute;
		GLint texCoordAttribute;

		GLsizei NumIndices;
	};
}