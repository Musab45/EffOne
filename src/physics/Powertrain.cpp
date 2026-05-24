#include "Powertrain.h"
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

// Approximate F1 torque curve: peaks ~450N·m at 10500 RPM
static const float TORQUE_RPM[]   = {0,2000,4000,6000,8000,10000,12000,14000,15000};
static const float TORQUE_NM[]    = {200,280,350,400,440,450,  420,  370,  300};
static const int   TORQUE_POINTS  = 9;

constexpr float Powertrain::GEAR_RATIOS[9];

float Powertrain::torqueCurve(float rpm) const {
    rpm = std::clamp(rpm, 0.0f, RPM_LIMIT);
    for (int i = 0; i < TORQUE_POINTS-1; ++i) {
        if (rpm <= TORQUE_RPM[i+1]) {
            float t = (rpm - TORQUE_RPM[i]) / (TORQUE_RPM[i+1] - TORQUE_RPM[i]);
            return TORQUE_NM[i] + t*(TORQUE_NM[i+1]-TORQUE_NM[i]);
        }
    }
    return TORQUE_NM[TORQUE_POINTS-1];
}

float Powertrain::driveTorque(float throttle, float rpm) const {
    return torqueCurve(rpm) * throttle;
}

void Powertrain::update(VehicleState& s, float dt) const {
    int g = std::clamp(s.gear, 1, 8);
    float ratio = GEAR_RATIOS[g] * FINAL_DRIVE;

    // Wheel speed from car speed (assume no slip for RPM estimate)
    float carSpeed = std::max(glm::length(s.velocity), 0.0f);
    float targetRpm = (carSpeed / WHEEL_RADIUS) * ratio * (60.0f/(2.0f*3.14159f));
    // Blend RPM toward wheel-driven value
    s.rpm = std::clamp(
        s.rpm + (targetRpm - s.rpm) * std::min(1.0f, dt * 8.0f),
        RPM_IDLE, RPM_LIMIT);

    // Gear change clamps
    if (s.gear < 8 && s.rpm > 13500.0f) { /* auto-upshift hint — player uses Shift */ }

    // Brake: compute wheel deceleration torque per wheel
    float brakeFront = s.brake * MAX_BRAKE_TORQUE * BRAKE_BIAS_FRONT;
    float brakeRear  = s.brake * MAX_BRAKE_TORQUE * (1.0f - BRAKE_BIAS_FRONT);

    // Longitudinal slip ratio per wheel (simplified: compare wheel speed to car speed)
    float carVelLong = carSpeed;
    for (int i = 0; i < 4; ++i) {
        float brakeTorque = (i < 2 ? brakeFront : brakeRear) * 0.5f;
        s.wheelSpeed[i] -= (brakeTorque / (200.0f * WHEEL_RADIUS)) * dt; // 200kg effective wheel inertia
        s.wheelSpeed[i]  = std::max(0.0f, s.wheelSpeed[i]);
        float wv = s.wheelSpeed[i] * WHEEL_RADIUS;
        float denom = std::max(carVelLong, 0.5f);
        s.slipRatio[i] = (wv - carVelLong) / denom;
    }
}
