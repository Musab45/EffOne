#pragma once
#include "../physics/VehicleState.h"

struct AudioEngine {
    void init();
    void update(const VehicleState& vs);
    void shutdown();

private:
    void* engine = nullptr; // ma_engine*, void* to avoid including miniaudio in header
    void* engineSound  = nullptr;
    void* tyreSound    = nullptr;
    void* windSound    = nullptr;
};
