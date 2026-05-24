#include "TrackCollision.h"
#include <algorithm>
#include <glm/gtc/quaternion.hpp>

WheelContact TrackCollision::castWheel(const glm::vec3& hubPos, float restLen, float maxTravel) const {
    WheelContact c;
    int idx  = track->nearestPoint(hubPos);
    const TrackPoint& tp = track->points[idx];
    // Use flat ground (y=0) for contact — spa elevation varies up to 25m
    // which breaks contact whenever the nearest waypoint changes.
    float groundY = 0.0f;
    float dist    = hubPos.y - groundY;

    if (dist < maxTravel) {
        c.hit         = true;
        c.point       = {hubPos.x, groundY, hubPos.z};
        c.normal      = tp.normal;
        // Compression relative to rest length — zero force at rest, positive when squished
        c.compression = std::max(0.0f, restLen - dist);
        c.gripCoeff   = tp.gripCoeff;
        c.surface     = tp.surface;
    }
    return c;
}

void TrackCollision::resolveWheels(VehicleState& s, RigidBody& rb, float springRate, float damperRate, float restLen) const {
    static const glm::vec3 OFFSETS[4] = {
        {-0.73f,-0.3f, 1.6f},{0.73f,-0.3f, 1.6f},
        {-0.73f,-0.3f,-1.5f},{0.73f,-0.3f,-1.5f}
    };
    glm::mat3 R = glm::mat3_cast(s.orientation);
    for (int i = 0; i < 4; ++i) {
        glm::vec3 hub = s.position + R * OFFSETS[i];
        auto c = castWheel(hub, restLen, 0.3f);
        if (c.hit) {
            s.suspensionCompression[i] = c.compression;
            float springForce = springRate * c.compression;
            // Damper: opposes hub vertical velocity (linear + angular contribution)
            glm::vec3 hubVel = s.velocity + glm::cross(s.angularVelocity, R * OFFSETS[i]);
            float damperForce = -damperRate * hubVel.y;
            float totalForce = std::max(0.0f, springForce + damperForce);
            s.fz[i] = std::max(s.fz[i], totalForce);
            rb.applyForce({0.0f, totalForce, 0.0f});
            // Hard floor: keep wheel above ground
            if (hub.y < c.point.y + 0.005f)
                s.position.y += (c.point.y + 0.005f - hub.y);
        } else {
            s.suspensionCompression[i] = 0.0f;
        }
    }
}
