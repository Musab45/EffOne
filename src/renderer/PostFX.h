#pragma once
#include <glad/glad.h>
#include "Shader.h"
#include "../physics/VehicleState.h"

struct PostFX {
    // HDR framebuffer (scene renders here first)
    GLuint hdrFBO = 0, hdrTex = 0, hdrDepth = 0;
    // Ping-pong for post passes
    GLuint pingFBO[2] = {}, pingTex[2] = {};
    GLuint dummyVAO   = 0;

    Shader hdrShader, blurShader, caShader, fxaaShader;

    int width = 1280, height = 720;

    void init(int w, int h);
    void bindHDR();            // call before scene render
    void apply(const VehicleState& vs, float speed); // call after scene render, before HUD
    void shutdown();

private:
    void drawFull();           // draw fullscreen triangle
    float prevSpeed = 0.0f;
};
