#include "TireModel.h"
#include <cmath>

float TireModel::magic(float B, float C, float D, float E, float slip) const {
    float Bslip = B * slip;
    return D * std::sin(C * std::atan(Bslip - E*(Bslip - std::atan(Bslip))));
}

float TireModel::lateralForce(float slipAngle, float fz) const {
    // Normalise fz: params tuned at 3000N reference load
    float loadFactor = fz / 3000.0f;
    float Dy = params.Dy * fz * loadFactor; // peak scales with load
    return magic(params.By, params.Cy, Dy, params.Ey, slipAngle);
}

float TireModel::longitudinalForce(float slipRatio, float fz) const {
    float loadFactor = fz / 3000.0f;
    float Dx = params.Dx * fz * loadFactor;
    return magic(params.Bx, params.Cx, Dx, params.Ex, slipRatio);
}
