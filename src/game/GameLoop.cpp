#include "GameLoop.h"
#include <GLFW/glfw3.h>

void GameLoop::run(std::function<void(double)> physicsTick,
                   std::function<void(double)> renderFrame) {
    double prev  = glfwGetTime();
    double accum = 0.0;
    while (true) {
        double now   = glfwGetTime();
        double delta = now - prev;
        prev = now;
        if (delta > 0.05) delta = 0.05; // clamp spiral of death
        accum += delta;
        while (accum >= PHYSICS_DT) {
            physicsTick(PHYSICS_DT);
            accum -= PHYSICS_DT;
        }
        renderFrame(accum / PHYSICS_DT);
    }
}
