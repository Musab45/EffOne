#include "PostFX.h"
#include <glm/glm.hpp>
#include <cmath>

static GLuint makeFBO(GLuint& tex, int w, int h, GLenum fmt = GL_RGBA16F) {
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D,0,fmt,w,h,0,GL_RGBA,GL_FLOAT,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,tex,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return fbo;
}

void PostFX::init(int w, int h) {
    width=w; height=h;
    // HDR FBO with depth
    glGenFramebuffers(1,&hdrFBO); glGenTextures(1,&hdrTex); glGenRenderbuffers(1,&hdrDepth);
    glBindTexture(GL_TEXTURE_2D,hdrTex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,w,h,0,GL_RGBA,GL_FLOAT,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindRenderbuffer(GL_RENDERBUFFER,hdrDepth);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,w,h);
    glBindFramebuffer(GL_FRAMEBUFFER,hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,hdrTex,0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,hdrDepth);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    // Ping-pong
    pingFBO[0]=makeFBO(pingTex[0],w,h);
    pingFBO[1]=makeFBO(pingTex[1],w,h);
    // Dummy VAO for fullscreen triangle
    glGenVertexArrays(1,&dummyVAO);

    hdrShader.load("assets/shaders/fullscreen.vert",    "assets/shaders/post_hdr.frag");
    blurShader.load("assets/shaders/fullscreen.vert",   "assets/shaders/post_motionblur.frag");
    caShader.load("assets/shaders/fullscreen.vert",     "assets/shaders/post_ca.frag");
    fxaaShader.load("assets/shaders/fullscreen.vert",   "assets/shaders/post_fxaa.frag");
}

void PostFX::bindHDR() {
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
}

void PostFX::drawFull() {
    glBindVertexArray(dummyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void PostFX::apply(const VehicleState& vs, float speed) {
    glDisable(GL_DEPTH_TEST);

    float totalG = glm::length(vs.acceleration) / 9.81f;

    // Pass 1: HDR → ping[0]
    glBindFramebuffer(GL_FRAMEBUFFER, pingFBO[0]);
    hdrShader.bind();
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, hdrTex);
    hdrShader.setInt("uScene", 0);
    hdrShader.setFloat("uExposure", 1.2f);
    drawFull();

    // Pass 2: Motion blur ping[0] → ping[1]
    glBindFramebuffer(GL_FRAMEBUFFER, pingFBO[1]);
    blurShader.bind();
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, pingTex[0]);
    blurShader.setInt("uScene", 0);
    float blurStrength = glm::clamp(speed / 300.0f, 0.0f, 1.0f);
    blurShader.setFloat("uStrength", blurStrength * 0.018f);
    blurShader.setVec2("uVelocity", 0.0f, 0.0f);
    drawFull();

    // Pass 3: Chromatic aberration ping[1] → ping[0]
    glBindFramebuffer(GL_FRAMEBUFFER, pingFBO[0]);
    caShader.bind();
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, pingTex[1]);
    caShader.setInt("uScene", 0);
    caShader.setFloat("uStrength", glm::clamp(totalG / 5.0f, 0.0f, 1.0f));
    drawFull();

    // Pass 4: FXAA → screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    fxaaShader.bind();
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, pingTex[0]);
    fxaaShader.setInt("uScene", 0);
    fxaaShader.setVec2("uTexelSize", 1.0f/width, 1.0f/height);
    drawFull();

    glEnable(GL_DEPTH_TEST);
    prevSpeed = speed;
}

void PostFX::shutdown() {
    glDeleteFramebuffers(1,&hdrFBO); glDeleteTextures(1,&hdrTex);
    glDeleteRenderbuffers(1,&hdrDepth);
    glDeleteFramebuffers(2,pingFBO); glDeleteTextures(2,pingTex);
    glDeleteVertexArrays(1,&dummyVAO);
}
