#pragma once
#include "../physics/VehicleState.h"

struct GLFWwindow;

struct HUD {
    bool showTelemetry = false;

    void init(GLFWwindow* window);
    void beginFrame();
    void drawTelemetry(const VehicleState& s);
    void endFrame();
    void shutdown();
};
