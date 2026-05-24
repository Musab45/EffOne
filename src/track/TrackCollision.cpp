#include "TrackCollision.h"
#include <algorithm>
#include <glm/gtc/quaternion.hpp>

WheelContact TrackCollision::castWheel(const glm::vec3& hubPos, float maxTravel) const {
    WheelContact c;
    // Find nearest track point and use its Y as ground height
    // (Flat track simplification — Task 14 replaces with mesh raycast for Spa)
    int idx  = track->nearestPoint(hubPos);
    const TrackPoint& tp = track->points[idx];
    float groundY = tp.position.y;
    float dist    = hubPos.y - groundY;

    if (dist < maxTravel) {
        c.hit         = true;
        c.point       = {hubPos.x, groundY, hubPos.z};
        c.normal      = tp.normal;
        c.compression = std::max(0.0f, maxTravel - dist);
        c.gripCoeff   = tp.gripCoeff;
        c.surface     = tp.surface;
    }
    return c;
}

void TrackCollision::resolveWheels(VehicleState& s, float springRate, float restLen) const {
    static const glm::vec3 OFFSETS[4] = {
        {-0.73f,-0.3f, 1.6f},{0.73f,-0.3f, 1.6f},
        {-0.73f,-0.3f,-1.5f},{0.73f,-0.3f,-1.5f}
    };
    glm::mat3 R = glm::mat3_cast(s.orientation);
    for (int i = 0; i < 4; ++i) {
        glm::vec3 hub = s.position + R * OFFSETS[i];
        auto c = castWheel(hub, 0.3f);
        if (c.hit) {
            s.suspensionCompression[i] = c.compression;
            s.fz[i] = springRate * c.compression;
            // Push car up (keep above ground)
            if (hub.y < c.point.y + 0.01f)
                s.position.y += (c.point.y + 0.01f - hub.y) * 0.8f;
        } else {
            s.suspensionCompression[i] = 0.0f;
            s.fz[i] = 0.0f;
        }
    }
}
