#pragma once
#include "../physics/VehicleState.h"

struct GLFWwindow;

struct HUD {
    bool showTelemetry = false;

    void init(GLFWwindow* window);
    void beginFrame();
    void drawTelemetry(const VehicleState& s);
    void drawRaceOverlay(const VehicleState& vs, float lapTime, float bestLap,
                         float sector[3], float bestSector[3], int screenW, int screenH);
    void endFrame();
    void shutdown();
};
