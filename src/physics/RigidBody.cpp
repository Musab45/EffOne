#include "RigidBody.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Define static inertia constants here to avoid MSVC constexpr FP issues
const float RigidBody::Ixx = RigidBody::MASS * (1.0f*1.0f + 2.0f*2.0f) / 12.0f;
const float RigidBody::Iyy = RigidBody::MASS * (5.0f*5.0f + 2.0f*2.0f) / 12.0f;
const float RigidBody::Izz = RigidBody::MASS * (5.0f*5.0f + 1.0f*1.0f) / 12.0f;

void RigidBody::applyForce(const glm::vec3& f)  { forceAccum  += f; }
void RigidBody::applyTorque(const glm::vec3& t) { torqueAccum += t; }
void RigidBody::reset()                          { forceAccum = torqueAccum = {0,0,0}; }

void RigidBody::applyForceAtPoint(const glm::vec3& force, const glm::vec3& point) {
    forceAccum  += force;
    torqueAccum += glm::cross(point, force);
}

void RigidBody::integrate(VehicleState& s, float dt) {
    // Linear: semi-implicit Euler (stable for linear dynamics)
    glm::vec3 accel = forceAccum / MASS;
    s.acceleration  = accel;
    s.velocity     += accel * dt;
    s.position     += s.velocity * dt;

    // Angular: RK4 on angular velocity
    glm::mat3 R    = glm::mat3_cast(s.orientation);
    glm::mat3 Iloc = glm::mat3(Ixx,0,0, 0,Iyy,0, 0,0,Izz);
    glm::mat3 I    = R * Iloc * glm::transpose(R);
    glm::mat3 Iinv = glm::inverse(I);

    auto angAccel = [&](const glm::vec3& w) -> glm::vec3 {
        return Iinv * (torqueAccum - glm::cross(w, I*w));
    };

    glm::vec3 k1 = angAccel(s.angularVelocity);
    glm::vec3 k2 = angAccel(s.angularVelocity + k1*(dt*0.5f));
    glm::vec3 k3 = angAccel(s.angularVelocity + k2*(dt*0.5f));
    glm::vec3 k4 = angAccel(s.angularVelocity + k3*dt);
    s.angularVelocity += (k1 + 2.0f*k2 + 2.0f*k3 + k4) * (dt/6.0f);

    // Integrate orientation from angular velocity
    glm::quat wQuat(0.0f, s.angularVelocity.x * 0.5f,
                          s.angularVelocity.y * 0.5f,
                          s.angularVelocity.z * 0.5f);
    s.orientation += wQuat * s.orientation * dt;
    s.orientation  = glm::normalize(s.orientation);
}
