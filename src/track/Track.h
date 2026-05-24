#pragma once
#include <glm/glm.hpp>
#include <vector>

enum class SurfaceType { ASPHALT, KERB, GRAVEL, GRASS };

struct TrackPoint {
    glm::vec3   position;
    glm::vec3   tangent;
    glm::vec3   normal   = {0,1,0};
    float       widthLeft  = 7.0f;
    float       widthRight = 7.0f;
    float       banking    = 0.0f;
    SurfaceType surface    = SurfaceType::ASPHALT;
    float       gripCoeff  = 1.0f;
};

struct Track {
    std::vector<TrackPoint> points;

    void         buildCircle(float radius, int segments); // flat test track
    TrackPoint   sample(float t) const;  // t in [0,1], Catmull-Rom interpolated
    int          nearestPoint(const glm::vec3& pos) const;
    float        lapProgress(const glm::vec3& pos) const; // 0..1
    void         loadSpa(const char* path); // loads binary .spa file
};
