#include "Track.h"
#include <cmath>
#include <limits>
#include <fstream>
#include <iostream>
#include <glm/gtx/norm.hpp>

void Track::buildCircle(float radius, int segments) {
    points.clear();
    for (int i = 0; i < segments; ++i) {
        float angle = (float)i / segments * 2.0f * 3.14159f;
        TrackPoint p;
        p.position = {std::cos(angle)*radius, 0.0f, std::sin(angle)*radius};
        float nextAngle = (float)(i+1) / segments * 2.0f * 3.14159f;
        glm::vec3 nextPos = {std::cos(nextAngle)*radius, 0.0f, std::sin(nextAngle)*radius};
        p.tangent  = glm::normalize(nextPos - p.position);
        p.normal   = {0,1,0};
        points.push_back(p);
    }
}

static glm::vec3 catmullRom(const glm::vec3& p0, const glm::vec3& p1,
                              const glm::vec3& p2, const glm::vec3& p3, float t) {
    float t2=t*t, t3=t2*t;
    return 0.5f*(2.0f*p1 + (-p0+p2)*t + (2.0f*p0-5.0f*p1+4.0f*p2-p3)*t2
                + (-p0+3.0f*p1-3.0f*p2+p3)*t3);
}

TrackPoint Track::sample(float t) const {
    int n = (int)points.size();
    float ft = t * n;
    int   i  = (int)ft;
    float f  = ft - i;
    auto get = [&](int idx) { return points[((idx % n) + n) % n].position; };
    TrackPoint out;
    out.position = catmullRom(get(i-1), get(i), get(i+1), get(i+2), f);
    // Tangent: finite difference
    float eps = 0.001f;
    glm::vec3 pA = catmullRom(get(i-1),get(i),get(i+1),get(i+2), f-eps);
    glm::vec3 pB = catmullRom(get(i-1),get(i),get(i+1),get(i+2), f+eps);
    out.tangent   = glm::normalize(pB - pA);
    out.normal    = points[i % n].normal;
    out.gripCoeff = points[i % n].gripCoeff;
    out.surface   = points[i % n].surface;
    return out;
}

int Track::nearestPoint(const glm::vec3& pos) const {
    int best = 0; float bestD = std::numeric_limits<float>::max();
    for (int i = 0; i < (int)points.size(); ++i) {
        float d = glm::length2(points[i].position - pos);
        if (d < bestD) { bestD = d; best = i; }
    }
    return best;
}

float Track::lapProgress(const glm::vec3& pos) const {
    return (float)nearestPoint(pos) / (float)points.size();
}

void Track::loadSpa(const char* path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) {
        std::cerr << "Track::loadSpa: cannot open " << path << "\n";
        return;
    }
    int cnt = 0;
    f.read((char*)&cnt, sizeof(cnt));
    if (cnt <= 0 || cnt > 100000) {
        std::cerr << "Track::loadSpa: invalid point count " << cnt << "\n";
        return;
    }
    points.clear();
    for (int i = 0; i < cnt; ++i) {
        // Read BinaryPoint struct (matches tools/track_builder layout)
        struct BP { float x,y,z,tx,ty,tz,nx,ny,nz,wL,wR,bank; int surf; float grip; };
        BP bp; f.read((char*)&bp, sizeof(bp));
        TrackPoint tp;
        tp.position   = {bp.x,bp.y,bp.z};
        tp.tangent    = {bp.tx,bp.ty,bp.tz};
        tp.normal     = {bp.nx,bp.ny,bp.nz};
        tp.widthLeft  = bp.wL; tp.widthRight = bp.wR;
        tp.banking    = bp.bank;
        tp.surface    = (SurfaceType)bp.surf;
        tp.gripCoeff  = bp.grip;
        points.push_back(tp);
    }
}
