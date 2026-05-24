#pragma once

// Pacejka Magic Formula '94 tire model.
// Computes lateral force (from slip angle) and longitudinal force (from slip ratio).
// Parameters tuned to approximate a modern F1 slick tire.
struct TireParams {
    // Lateral (cornering)
    float By = 10.0f, Cy = 1.3f, Dy = 1.0f, Ey = -1.0f;
    // Longitudinal (drive/brake)
    float Bx = 12.0f, Cx = 1.65f, Dx = 1.0f, Ex = -0.5f;
};

struct TireModel {
    TireParams params;

    // fz: normal load (N). Peak grip scales linearly with fz.
    // slipAngle: lateral slip angle (radians). Positive = understeer direction.
    // Returns lateral force (N) — positive pushes car toward centre of turn.
    float lateralForce(float slipAngle, float fz) const;

    // slipRatio: longitudinal slip kappa [-1..+1]. -1 = lockup, +1 = wheelspin.
    // Returns longitudinal force (N) — positive = forward traction.
    float longitudinalForce(float slipRatio, float fz) const;

private:
    float magic(float B, float C, float D, float E, float slip) const;
};
