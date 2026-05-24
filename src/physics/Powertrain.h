#pragma once
#include "VehicleState.h"

struct Powertrain {
    // 8-speed gear ratios (modern F1)
    static constexpr float GEAR_RATIOS[9] = {0, 3.30f,2.10f,1.61f,1.29f,1.07f,0.90f,0.77f,0.67f};
    static constexpr float FINAL_DRIVE    = 8.0f;
    static constexpr float WHEEL_RADIUS   = 0.33f; // metres
    static constexpr float RPM_IDLE       = 800.0f;
    static constexpr float RPM_LIMIT      = 15000.0f;
    static constexpr float MAX_BRAKE_TORQUE = 3000.0f; // N·m per axle
    static constexpr float BRAKE_BIAS_FRONT = 0.57f;

    // Returns drive torque at wheel (N·m) given throttle and current rpm.
    float driveTorque(float throttle, float rpm) const;

    // Advance powertrain state: rpm, wheel speeds, gear changes.
    void update(VehicleState& s, float dt) const;

private:
    float torqueCurve(float rpm) const; // lookup table interpolation
};
