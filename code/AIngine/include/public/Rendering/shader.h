#pragma once

#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <string>
namespace AIngine::Rendering {
	class GLShader final {
	public:
		GLShader(std::string const &path, int type);

		~GLShader();

		GLuint GetID();

	private:
		GLuint ID;
	};

	class GLShaderProgram final {
	public:
		GLShaderProgram(std::string const &vertexPath, std::string const &fragmentPath);

		~GLShaderProgram();

		void Use();

		void    SetFloat(const GLchar *name, GLfloat value, GLboolean useShader = false);
		void    SetInteger(const GLchar *name, GLint value, GLboolean useShader = false);
		void    SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
		void    SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
		void    SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
		void    SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
		void    SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
		void    SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
		void    SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);

		GLuint GetID();

	private:
		GLuint ID;
	};
}