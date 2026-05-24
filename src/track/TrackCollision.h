#pragma once
#include "Track.h"
#include "../physics/VehicleState.h"
#include "../physics/RigidBody.h"

struct WheelContact {
    bool      hit         = false;
    glm::vec3 point       = {0,0,0};
    glm::vec3 normal      = {0,1,0};
    float     compression = 0.0f;   // metres
    float     gripCoeff   = 1.0f;
    SurfaceType surface   = SurfaceType::ASPHALT;
};

struct TrackCollision {
    const Track* track = nullptr;

    void init(const Track* t) { track = t; }

    // Cast ray downward from hubPos. Returns contact with nearest track surface.
    WheelContact castWheel(const glm::vec3& hubPos, float restLen, float maxTravel) const;

    // Update all 4 wheel contacts and write Fz + suspension into VehicleState.
    // Applies upward spring reaction force to the rigid body for each contact.
    void resolveWheels(VehicleState& s, RigidBody& rb, float springRate, float restLength) const;
};
