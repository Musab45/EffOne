#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct VehicleState {
    // rigid body
    glm::vec3 position        = {0, 0.3f, 0};
    glm::vec3 velocity        = {0, 0, 0};
    glm::quat orientation     = glm::quat(1, 0, 0, 0); // identity
    glm::vec3 angularVelocity = {0, 0, 0};
    glm::vec3 acceleration    = {0, 0, 0}; // world space, for camera + HUD

    // wheels [FL, FR, RL, RR]
    float wheelSpeed[4]            = {0,0,0,0}; // rad/s
    float suspensionCompression[4] = {0,0,0,0}; // metres
    float slipAngle[4]             = {0,0,0,0}; // radians
    float slipRatio[4]             = {0,0,0,0}; // longitudinal kappa
    float fz[4]                    = {0,0,0,0}; // normal load (N)

    // powertrain
    float throttle = 0.0f; // 0..1
    float brake    = 0.0f; // 0..1
    float steer    = 0.0f; // -1..+1 (wheel angle)
    float rpm      = 800.0f;
    int   gear     = 1;
    bool  drsOpen  = false;
};
