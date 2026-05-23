#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct Mesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    int    indexCount = 0;

    void upload(const std::vector<Vertex>& verts, const std::vector<unsigned>& indices);
    void draw() const;
    void free();

    static Mesh makeFlat(float halfSize, int divisions); // flat grid for testing
    static Mesh makeBox(float hx, float hy, float hz);   // simple box mesh
};
