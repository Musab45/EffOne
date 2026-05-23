#pragma once
#include <functional>

// dt_physics = 1/1000 s. Calls physicsTick repeatedly until caught up,
// then calls renderFrame with interpolation alpha [0,1].
struct GameLoop {
    static constexpr double PHYSICS_DT = 1.0 / 1000.0;
    bool shouldQuit = false;

    void run(std::function<void(double dt)>    physicsTick,
             std::function<void(double alpha)> renderFrame);
};
