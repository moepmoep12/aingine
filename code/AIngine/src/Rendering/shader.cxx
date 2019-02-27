#include "Rendering/shader.h"
#include "readfile.h"

#include <glm/gtc/type_ptr.hpp>

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

    CheckError(ID, _TYPE::PROGRAM);
}

GLShaderProgram::~GLShaderProgram() {
    glDeleteProgram(ID);
}

void GLShaderProgram::Use() {
    glUseProgram(ID);
}

void GLShaderProgram::SetFloat4x4(std::string const &name, glm::mat4x4 value) {
    GLint attrLocation = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(attrLocation, 1, GL_FALSE, glm::value_ptr(value));
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

