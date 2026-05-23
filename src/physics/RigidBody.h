#pragma once
#include "VehicleState.h"
#include <glm/glm.hpp>

// 6-DOF rigid body, RK4 integrated.
// mass = 798 kg (F1 car + driver).
// Caller accumulates forces/torques each tick then calls integrate().
struct RigidBody {
    static constexpr float MASS = 798.0f;

    // Inertia tensor (diagonal approximation: L=5m W=2m H=1m box)
    static const float Ixx; // MASS*(1^2 + 2^2)/12
    static const float Iyy; // MASS*(5^2 + 2^2)/12
    static const float Izz; // MASS*(5^2 + 1^2)/12

    glm::vec3 forceAccum  = {0,0,0}; // reset each tick
    glm::vec3 torqueAccum = {0,0,0};

    void applyForceAtPoint(const glm::vec3& force, const glm::vec3& pointRelCOG);
    void applyForce(const glm::vec3& force);
    void applyTorque(const glm::vec3& torque);
    void integrate(VehicleState& s, float dt);
    void reset();
};
