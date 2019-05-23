#include "Rendering/shader.h"
#include "readfile.h"

#include <glm/gtc/type_ptr.hpp>

namespace AIngine::Rendering {
	enum _TYPE {
		SHADER,
		PROGRAM
	};

	void CheckError(unsigned int ID, _TYPE type);

	GLShader::GLShader(std::string const &path, int type) {
		std::string code;
		ReadFile(path, code);
		const char *shaderCode = code.c_str();
		ID = glCreateShader(type);
		glShaderSource(ID, 1, &shaderCode, NULL);
		glCompileShader(ID);
		CheckError(ID, _TYPE::SHADER);
	}

	GLShader::~GLShader() {
		glDeleteShader(ID);
	}

	GLuint GLShader::GetID() {
		return ID;
	}

	GLShaderProgram::GLShaderProgram(std::string const &vertexPath, std::string const &fragmentPath) {
		// vertex shader
		GLShader vertex(vertexPath, GL_VERTEX_SHADER);
		GLShader fragment(fragmentPath, GL_FRAGMENT_SHADER);

		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex.GetID());
		glAttachShader(ID, fragment.GetID());
		glLinkProgram(ID);

		glDeleteShader(vertex.GetID());
		glDeleteShader(fragment.GetID());

		CheckError(ID, _TYPE::PROGRAM);
	}

	GLShaderProgram::~GLShaderProgram() {
		glDeleteProgram(ID);
	}

	void GLShaderProgram::Use() {
		glUseProgram(ID);
	}

	void GLShaderProgram::SetFloat(const GLchar *name, GLfloat value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform1f(glGetUniformLocation(this->ID, name), value);
	}
	void GLShaderProgram::SetInteger(const GLchar *name, GLint value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform1i(glGetUniformLocation(this->ID, name), value);
	}
	void GLShaderProgram::SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform2f(glGetUniformLocation(this->ID, name), x, y);
	}
	void GLShaderProgram::SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
	}
	void GLShaderProgram::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
	}
	void GLShaderProgram::SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
	}
	void GLShaderProgram::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
	}
	void GLShaderProgram::SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
	}
	void GLShaderProgram::SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
	{
		if (useShader)
			this->Use();
		glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	GLuint GLShaderProgram::GetID() {
		return ID;
	}

	void CheckError(unsigned int ID, _TYPE type) {
		int success = -1;
		char infoLog[1024];

		switch (type) {
		case PROGRAM:
			glGetProgramiv(ID, GL_LINK_STATUS, &success);
			break;
		case SHADER:
			glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		}

		if (!success) {
			switch (type) {
			case PROGRAM:
				glGetProgramInfoLog(ID, 1024, NULL, infoLog);
				break;
			case SHADER:
				glGetShaderInfoLog(ID, 1024, NULL, infoLog);
			}
			std::string msg = "Compilation failed: ";
			msg += infoLog;
			throw std::runtime_error(msg);
		}
	}
}
