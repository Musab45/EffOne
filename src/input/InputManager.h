#pragma once
#include <GLFW/glfw3.h>

struct InputState {
    float throttle = 0.0f;  // 0..1  (W held = 1)
    float brake    = 0.0f;  // 0..1  (S held = 1)
    float steer    = 0.0f;  // -1..+1 (A=-1, D=+1)
    bool  gearUp   = false; // Shift pressed this tick
    bool  gearDown = false; // Ctrl pressed this tick
    bool  drs      = false; // Space pressed this tick
    bool  reset    = false; // R pressed this tick
    bool  quit     = false; // Esc pressed
    bool  toggleTelemetry = false; // F1 pressed this tick
};

struct InputManager {
    void       init(GLFWwindow* window);
    InputState poll();          // call once per physics tick
private:
    GLFWwindow* win = nullptr;
    bool prevShift = false, prevCtrl = false, prevSpace = false;
    bool prevR = false, prevF1 = false;
};
