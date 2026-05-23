#include "Mesh.h"

void Mesh::upload(const std::vector<Vertex>& verts, const std::vector<unsigned>& indices) {
    indexCount = (int)indices.size();
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::free() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

Mesh Mesh::makeFlat(float half, int divs) {
    std::vector<Vertex> verts;
    std::vector<unsigned> idx;
    float step = (half*2.0f)/divs;
    for (int z = 0; z <= divs; ++z)
        for (int x = 0; x <= divs; ++x) {
            Vertex v;
            v.position = {-half + x*step, 0.0f, -half + z*step};
            v.normal   = {0,1,0};
            v.uv       = {(float)x/divs, (float)z/divs};
            verts.push_back(v);
        }
    for (int z = 0; z < divs; ++z)
        for (int x = 0; x < divs; ++x) {
            unsigned tl = z*(divs+1)+x, tr=tl+1, bl=tl+(divs+1), br=bl+1;
            idx.insert(idx.end(), {tl,bl,tr, tr,bl,br});
        }
    Mesh m; m.upload(verts, idx); return m;
}

Mesh Mesh::makeBox(float hx, float hy, float hz) {
    std::vector<Vertex> verts;
    std::vector<unsigned> idx;
    // 6 faces, 4 verts each
    auto face = [&](glm::vec3 n, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
        unsigned base = (unsigned)verts.size();
        for (auto& p : {p0,p1,p2,p3}) verts.push_back({p, n, {0,0}});
        idx.insert(idx.end(), {base,base+1,base+2, base,base+2,base+3});
    };
    face({0,0,1},  {-hx,-hy, hz},{hx,-hy, hz},{hx, hy, hz},{-hx, hy, hz}); // +Z
    face({0,0,-1}, {hx,-hy,-hz},{-hx,-hy,-hz},{-hx, hy,-hz},{hx, hy,-hz}); // -Z
    face({1,0,0},  {hx,-hy, hz},{hx,-hy,-hz},{hx, hy,-hz},{hx, hy, hz});   // +X
    face({-1,0,0}, {-hx,-hy,-hz},{-hx,-hy, hz},{-hx, hy, hz},{-hx, hy,-hz});// -X
    face({0,1,0},  {-hx, hy, hz},{hx, hy, hz},{hx, hy,-hz},{-hx, hy,-hz}); // +Y
    face({0,-1,0}, {-hx,-hy,-hz},{hx,-hy,-hz},{hx,-hy, hz},{-hx,-hy, hz}); // -Y
    Mesh m; m.upload(verts, idx); return m;
}
