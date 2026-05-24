#pragma once
#include "VehicleState.h"
#include <glm/glm.hpp>

// Wheel positions relative to COG (metres):
// FL=(-0.73, -0.3, 1.6)  FR=(+0.73, -0.3, 1.6)
// RL=(-0.73, -0.3,-1.5)  RR=(+0.73, -0.3,-1.5)
extern const glm::vec3 WHEEL_OFFSETS[4];

struct Suspension {
    float springRate  = 80000.0f; // N/m  (very stiff, F1-spec)
    float damperRate  = 4000.0f;  // N·s/m
    float restLength  = 0.05f;    // metres at rest
    float maxTravel   = 0.08f;    // maximum compression

    // Computes Fz[4] from compression and velocity at each corner.
    // Also computes weight transfer from longitudinal/lateral acceleration.
    void computeLoads(VehicleState& s, const glm::vec3& accelWorld) const;
};
