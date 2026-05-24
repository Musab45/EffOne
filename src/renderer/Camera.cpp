#include "Camera.h"
#include <glm/gtx/quaternion.hpp>
#include <cmath>

static float lerp(float a, float b, float t) { return a + (b-a)*t; }

void Camera::update(const VehicleState& s, float dt) {
    // World-space acceleration → local-space
    glm::mat3 R  = glm::mat3_cast(s.orientation);
    glm::mat3 Rt = glm::transpose(R);
    glm::vec3 localAccel = Rt * s.acceleration;

    float ax = localAccel.z; // longitudinal (forward = -Z, braking = negative ax)
    float ay = localAccel.x; // lateral

    float targetPitch = -ax * BRAKE_DIVE_FACTOR;   // braking → nose pitch down
    float targetRoll  =  ay * CORNERING_ROLL_FACTOR;

    float totalG = glm::length(s.acceleration) / 9.81f;
    float targetFov = FOV_BASE - totalG * FOV_SQUEEZE;

    float alpha = std::min(1.0f, dt * SPRING_STIFFNESS);
    pitchOffset = lerp(pitchOffset, targetPitch, alpha);
    rollOffset  = lerp(rollOffset,  targetRoll,  alpha);
    currentFov  = lerp(currentFov,  targetFov,   alpha * 0.5f);

    // High-frequency bump from kerbs: decay timer
    if (bumpTimer > 0.0f) {
        bumpTimer -= dt;
        bumpY = std::sin(bumpTimer * 40.0f) * 0.015f * bumpTimer;
    } else {
        bumpY = 0.0f;
    }
}

void Camera::triggerBump(float intensity) { bumpTimer = intensity * 0.3f; }

glm::mat4 Camera::viewMatrix(const VehicleState& s) const {
    glm::mat3 R = glm::mat3_cast(s.orientation);
    glm::vec3 eye = s.position + R * localOffset + glm::vec3(0, bumpY, 0);

    // Build camera orientation: car orientation + pitch/roll offsets
    glm::quat pitchQ = glm::angleAxis(pitchOffset, glm::vec3(1,0,0));
    glm::quat rollQ  = glm::angleAxis(rollOffset,  glm::vec3(0,0,1));
    glm::quat camOri = s.orientation * pitchQ * rollQ;

    glm::vec3 forward = camOri * glm::vec3(0,0,-1);
    glm::vec3 up      = camOri * glm::vec3(0,1,0);
    return glm::lookAt(eye, eye + forward, up);
}
