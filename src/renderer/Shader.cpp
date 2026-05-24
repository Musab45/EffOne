#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

static std::string readFile(const char* path) {
    std::ifstream f(path);
    if (!f) { std::cerr << "Shader not found: " << path << "\n"; return ""; }
    std::stringstream ss; ss << f.rdbuf(); return ss.str();
}

static GLuint compile(GLenum type, const std::string& src) {
    if (src.empty()) { std::cerr << "Empty shader source\n"; return 0; }
    const char* c = src.c_str();
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &c, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << "Shader compile error:\n" << log;
    }
    return s;
}

void Shader::load(const char* vertPath, const char* fragPath) {
    GLuint v = compile(GL_VERTEX_SHADER,   readFile(vertPath));
    GLuint f = compile(GL_FRAGMENT_SHADER, readFile(fragPath));
    id = glCreateProgram();
    glAttachShader(id, v); glAttachShader(id, f);
    glLinkProgram(id);
    GLint ok; glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) { char log[512]; glGetProgramInfoLog(id, 512, nullptr, log); std::cerr << log; }
    glDeleteShader(v); glDeleteShader(f);
}

void Shader::bind()                                         const { glUseProgram(id); }
void Shader::setMat4(const char* n, const glm::mat4& m)    const { glUniformMatrix4fv(glGetUniformLocation(id,n),1,GL_FALSE,glm::value_ptr(m)); }
void Shader::setVec3(const char* n, const glm::vec3& v)    const { glUniform3fv(glGetUniformLocation(id,n),1,glm::value_ptr(v)); }
void Shader::setFloat(const char* n, float v)               const { glUniform1f(glGetUniformLocation(id,n),v); }
void Shader::setInt(const char* n, int v)                   const { glUniform1i(glGetUniformLocation(id,n),v); }
void Shader::setVec2(const char* n, float x, float y)       const { glUniform2f(glGetUniformLocation(id,n),x,y); }
