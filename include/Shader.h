#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    GLuint ID;
    Shader();
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    void use() const { glUseProgram(ID); }

    // Set uniform functions now unambiguous
    void setFloat(const std::string &name, float value) const;
    void setInt(const std::string &name, int value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;
    void setVec4(const std::string &name, const glm::vec4 &vec) const;

private:
    std::string loadSource(const std::string &path);
    GLuint compileShader(GLenum type, const std::string &source);
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif
