#include "TrackCollision.h"
#include <algorithm>
#include <glm/gtc/quaternion.hpp>

WheelContact TrackCollision::castWheel(const glm::vec3& hubPos, float restLen, float maxTravel) const {
    WheelContact c;
    int idx  = track->nearestPoint(hubPos);
    const TrackPoint& tp = track->points[idx];
    float groundY = tp.position.y;
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

void TrackCollision::resolveWheels(VehicleState& s, RigidBody& rb, float springRate, float restLen) const {
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
            // Don't wipe out the aero + weight-transfer load that
            // Suspension::computeLoads + downforce accumulation just set.
            // Take the larger of the two so contact also accumulates with aero.
            s.fz[i] = std::max(s.fz[i], springForce);
            // Apply upward spring reaction to the body so gravity is balanced.
            rb.applyForce({0.0f, springForce, 0.0f});
            // Push car up (keep above ground)
            if (hub.y < c.point.y + 0.01f)
                s.position.y += (c.point.y + 0.01f - hub.y) * 0.8f;
        } else {
            s.suspensionCompression[i] = 0.0f;
            // No contact: aero-only Fz can remain but there is no spring load.
            // Leave whatever Suspension/aero accumulated; nothing to add.
        }
    }
}
