#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() : ID(0) {}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    std::string vSrc = loadSource(vertexPath);
    std::string fSrc = loadSource(fragmentPath);

    GLuint vShader = compileShader(GL_VERTEX_SHADER, vSrc);
    GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fSrc);

    // Create program
    ID = glCreateProgram();
    glAttachShader(ID, vShader);
    glAttachShader(ID, fShader);
    glLinkProgram(ID);

    // Check for linking errors
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vShader);
    glDeleteShader(fShader);
}

std::string Shader::loadSource(const std::string &path) {
    std::ifstream file(path);
    if(!file.is_open()){
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return std::string();
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(GLenum type, const std::string &source) {
    GLuint shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // Check compile errors
    checkCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return shader;
}

void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Shader compile error (" << type << "): " << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Program link error: " << infoLog << std::endl;
        }
    }
}

void Shader::setFloat(const std::string &name, float value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, value);
}

void Shader::setInt(const std::string &name, int value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, value);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(loc, 1, &vec[0]);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &vec) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform4fv(loc, 1, &vec[0]);
}
