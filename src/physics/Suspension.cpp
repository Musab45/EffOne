#include "Suspension.h"
#include <algorithm>
#include <glm/gtc/quaternion.hpp>

static constexpr float MASS      = 798.0f;
static constexpr float WHEELBASE = 3.1f;   // metres
static constexpr float TRACKWIDTH = 1.46f; // metres
static constexpr float H_COG     = 0.3f;   // height of COG (metres)

// Wheel positions relative to COG (metres):
// FL=(-0.73, -0.3, 1.6)  FR=(+0.73, -0.3, 1.6)
// RL=(-0.73, -0.3,-1.5)  RR=(+0.73, -0.3,-1.5)
const glm::vec3 WHEEL_OFFSETS[4] = {
    {-0.73f, -0.3f,  1.6f},
    { 0.73f, -0.3f,  1.6f},
    {-0.73f, -0.3f, -1.5f},
    { 0.73f, -0.3f, -1.5f},
};

void Suspension::computeLoads(VehicleState& s, const glm::vec3& accelWorld) const {
    // Static load (weight / 4 per corner)
    float staticFz = MASS * 9.81f / 4.0f;

    // Transform world-space acceleration into car-local frame so weight
    // transfer is computed against the car's longitudinal/lateral axes
    // regardless of yaw orientation.
    glm::mat3 R = glm::mat3_cast(s.orientation);
    glm::vec3 aLocal = glm::transpose(R) * accelWorld;

    // Weight transfer (car-local: forward = -Z, right = +X)
    float ax = -aLocal.z; // longitudinal (forward positive)
    float ay =  aLocal.x; // lateral      (right positive)
    float dFz_long = MASS * ax * H_COG / WHEELBASE * 0.5f;  // front/rear split
    float dFz_lat  = MASS * ay * H_COG / TRACKWIDTH * 0.5f; // left/right split

    // Apply: FL=0 FR=1 RL=2 RR=3
    // Braking (ax negative in Z) → nose dives → front gains load
    s.fz[0] = staticFz + dFz_long + dFz_lat;  // FL
    s.fz[1] = staticFz + dFz_long - dFz_lat;  // FR
    s.fz[2] = staticFz - dFz_long + dFz_lat;  // RL
    s.fz[3] = staticFz - dFz_long - dFz_lat;  // RR

    // Clamp: tires can't pull the car down
    for (int i = 0; i < 4; ++i)
        s.fz[i] = std::max(0.0f, s.fz[i]);

    // Suspension compression from fz (spring model inverse)
    for (int i = 0; i < 4; ++i)
        s.suspensionCompression[i] = std::min(s.fz[i] / springRate, maxTravel);
}
