#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

struct Shader {
    GLuint id = 0;
    void   load(const char* vertPath, const char* fragPath);
    void   bind() const;
    void   setMat4(const char* name, const glm::mat4& m) const;
    void   setVec3(const char* name, const glm::vec3& v) const;
    void   setFloat(const char* name, float v) const;
    void   setInt(const char* name, int v) const;
    void   setVec2(const char* name, float x, float y) const;
};
