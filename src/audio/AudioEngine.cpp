#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include "AudioEngine.h"
#include <glm/glm.hpp>
#include <cmath>
#include <algorithm>

static ma_engine* ENG = nullptr;
static ma_sound   engSound, tyrSound, wndSound;
static bool       engOk=false, tyrOk=false, wndOk=false;

void AudioEngine::init() {
    ENG = new ma_engine;
    if (ma_engine_init(nullptr, ENG) != MA_SUCCESS) { delete ENG; ENG=nullptr; return; }

    engOk = (ma_sound_init_from_file(ENG, "assets/audio/engine/engine_loop.wav",
        MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_LOOPING, nullptr, nullptr, &engSound) == MA_SUCCESS);
    tyrOk = (ma_sound_init_from_file(ENG, "assets/audio/sfx/tyre_squeal.wav",
        MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_LOOPING, nullptr, nullptr, &tyrSound) == MA_SUCCESS);
    wndOk = (ma_sound_init_from_file(ENG, "assets/audio/sfx/wind.wav",
        MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_LOOPING, nullptr, nullptr, &wndSound) == MA_SUCCESS);

    if (engOk) ma_sound_start(&engSound);
    if (tyrOk) ma_sound_start(&tyrSound);
    if (wndOk) ma_sound_start(&wndSound);
}

void AudioEngine::update(const VehicleState& vs) {
    if (!ENG) return;

    // Engine: pitch from RPM (1.0 = 6000 RPM reference)
    if (engOk) {
        float rpmNorm = vs.rpm / 6000.0f;
        float pitch   = std::clamp(rpmNorm, 0.5f, 2.5f);
        float engVol  = 0.3f + vs.throttle * 0.7f;
        ma_sound_set_pitch(&engSound, pitch);
        ma_sound_set_volume(&engSound, engVol);
    }

    // Tyre squeal: max slip angle across all wheels
    if (tyrOk) {
        float maxSlip = 0.0f;
        for (int i = 0; i < 4; ++i)
            maxSlip = std::max(maxSlip, std::abs(vs.slipAngle[i]));
        float squealVol = std::clamp((maxSlip - 0.05f) / 0.2f, 0.0f, 1.0f);
        ma_sound_set_volume(&tyrSound, squealVol);
    }

    // Wind: proportional to speed²
    if (wndOk) {
        float speed = glm::length(vs.velocity) * 3.6f; // km/h
        float windVol = std::clamp((speed / 300.0f) * (speed / 300.0f), 0.0f, 0.8f);
        ma_sound_set_volume(&wndSound, windVol);
    }
}

void AudioEngine::shutdown() {
    if (!ENG) return;
    if (engOk) ma_sound_uninit(&engSound);
    if (tyrOk) ma_sound_uninit(&tyrSound);
    if (wndOk) ma_sound_uninit(&wndSound);
    ma_engine_uninit(ENG);
    delete ENG; ENG = nullptr;
}
