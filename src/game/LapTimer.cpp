#include "LapTimer.h"
#include <algorithm>

// Spa sector boundaries as lap progress fractions (approximate FIA split points)
static constexpr float SECTOR_BOUNDS[2] = {0.33f, 0.66f}; // S1|S2, S2|S3

bool LapTimer::update(float t, float dt) {
    if (!started && t > 0.01f) started = true;
    if (!started) return false;

    lapTime += dt;
    sectorTime[currentSector] += dt;

    // Sector crossing
    if (currentSector == 0 && t >= SECTOR_BOUNDS[0]) {
        bestSector[0] = (bestSector[0] < 0.001f) ? sectorTime[0]
                       : std::min(bestSector[0], sectorTime[0]);
        sectorTime[1] = 0.0f;
        currentSector = 1;
    } else if (currentSector == 1 && t >= SECTOR_BOUNDS[1]) {
        bestSector[1] = (bestSector[1] < 0.001f) ? sectorTime[1]
                       : std::min(bestSector[1], sectorTime[1]);
        sectorTime[2] = 0.0f;
        currentSector = 2;
    } else if (currentSector == 2 && t < 0.05f) {
        // Crossed finish line
        bestSector[2] = (bestSector[2] < 0.001f) ? sectorTime[2]
                       : std::min(bestSector[2], sectorTime[2]);
        if (bestLap < 0.001f || lapTime < bestLap) bestLap = lapTime;
        lapTime = 0.0f; sectorTime[0] = 0.0f; currentSector = 0;
        return true; // new lap
    }
    return false;
}

void LapTimer::reset() {
    lapTime = bestLap = 0; currentSector = 0; started = false;
    for (int i=0;i<3;i++) { sectorTime[i]=0; bestSector[i]=0; }
}
