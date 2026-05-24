#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../physics/VehicleState.h"

struct Camera {
    // Tuning constants
    static constexpr float BRAKE_DIVE_FACTOR    = 0.04f;  // rad per m/s² longitudinal
    static constexpr float CORNERING_ROLL_FACTOR = 0.015f; // rad per m/s² lateral
    static constexpr float FOV_BASE             = 80.0f;  // degrees
    static constexpr float FOV_SQUEEZE          = 0.6f;   // degrees per G
    static constexpr float SPRING_STIFFNESS     = 12.0f;  // smoothing speed

    // Local offset of camera eye from car COG (cockpit position)
    glm::vec3 localOffset = {0.0f, 0.55f, 0.35f};

    // Current smoothed camera state
    float pitchOffset = 0.0f;
    float rollOffset  = 0.0f;
    float bumpY       = 0.0f;
    float currentFov  = FOV_BASE;
    float bumpTimer   = 0.0f;

    void update(const VehicleState& s, float dt);
    void triggerBump(float intensity);

    glm::mat4 viewMatrix(const VehicleState& s) const;
    float     fov() const { return currentFov; }
};
