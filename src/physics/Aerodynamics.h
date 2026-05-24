#pragma once
#include "VehicleState.h"
#include <glm/glm.hpp>

struct Aerodynamics {
    float rho    = 1.225f;  // air density kg/m³
    float CL     = 3.5f;    // lift (downforce) coefficient
    float CD     = 0.9f;    // drag coefficient
    float area   = 1.5f;    // reference area m²
    float drs_CD_reduction = 0.15f;  // fraction drag removed when DRS open
    float drs_CL_reduction = 0.10f;  // fraction downforce removed when DRS open

    // Returns downforce (N, positive = pushes car down).
    // Adds to s.fz[0..3] equally front/rear.
    // Returns drag force (N, opposes velocity) via dragForce out-param.
    float computeDownforce(const VehicleState& s) const;
    glm::vec3 computeDrag(const VehicleState& s) const;
};
