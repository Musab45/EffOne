#include "Aerodynamics.h"
#include <glm/glm.hpp>

float Aerodynamics::computeDownforce(const VehicleState& s) const {
    float v2   = glm::dot(s.velocity, s.velocity);
    float cl   = s.drsOpen ? CL * (1.0f - drs_CL_reduction) : CL;
    return 0.5f * rho * v2 * cl * area; // N downward
}

glm::vec3 Aerodynamics::computeDrag(const VehicleState& s) const {
    float speed = glm::length(s.velocity);
    if (speed < 0.01f) return {0,0,0};
    float cd  = s.drsOpen ? CD * (1.0f - drs_CD_reduction) : CD;
    float mag = 0.5f * rho * speed * speed * cd * area;
    return -(s.velocity / speed) * mag; // opposes velocity
}
