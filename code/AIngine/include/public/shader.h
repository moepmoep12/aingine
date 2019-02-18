#pragma once

#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <string>

class GLShader {
public:
    GLShader(std::string const &path, int type);

    ~GLShader();

    GLuint GetID();

private:
    GLuint ID;
};

class GLShaderProgram {
public:
    GLShaderProgram(std::string const &vertexPath, std::string const &fragmentPath);

    ~GLShaderProgram();

    void Use();

    void SetFloat4x4(std::string const &name, glm::mat4x4 value);

    GLuint GetID();

private:
    GLuint ID;
};


