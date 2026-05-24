#pragma once
#include <array>

struct LapTimer {
    float lapTime     = 0.0f;
    float bestLap     = 0.0f;
    float sectorTime[3]  = {};
    float bestSector[3]  = {};
    int   currentSector  = 0;  // 0, 1, 2
    float sectorStart    = 0.0f;
    bool  started        = false;

    // Call each physics tick with current lap progress [0..1] and dt
    // Returns true when a new lap is completed
    bool update(float lapProgress, float dt);

    void reset();
};
