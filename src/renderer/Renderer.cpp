#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

void Renderer::init() {
    sunDir = glm::normalize(glm::vec3(-0.4f,-1.0f,-0.3f));

    pbrShader.load("assets/shaders/pbr.vert", "assets/shaders/pbr.frag");
    shadowShader.load("assets/shaders/shadow.vert", "assets/shaders/shadow.frag");

    glGenFramebuffers(1, &shadowFBO);
    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowSize, shadowSize, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border[] = {1,1,1,1};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 Renderer::lightSpaceMat() const {
    glm::mat4 proj = glm::ortho(-200.0f,200.0f,-200.0f,200.0f,0.1f,500.0f);
    glm::mat4 view = glm::lookAt(-sunDir*150.0f, {0,0,0}, {0,1,0});
    return proj * view;
}

void Renderer::renderShadowPass(const Mesh& trackMesh, const glm::mat4& trackModel) {
    glViewport(0,0,shadowSize,shadowSize);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    shadowShader.bind();
    shadowShader.setMat4("uLightMVP", lightSpaceMat() * trackModel);
    trackMesh.draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderScene(const Camera& cam, const VehicleState& vs,
                            const Mesh& trackMesh, const Mesh& carMesh,
                            int w, int h) {
    glViewport(0,0,w,h);
    glClearColor(0.53f,0.81f,0.98f,1.0f); // sky blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj  = glm::perspective(glm::radians(cam.fov()), (float)w/h, 0.05f, 2000.0f);
    glm::mat4 view  = cam.viewMatrix(vs);
    glm::vec3 camPos= vs.position + glm::mat3_cast(vs.orientation) * cam.localOffset;
    glm::mat4 lMat  = lightSpaceMat();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

    pbrShader.bind();
    pbrShader.setMat4("uView",    view);
    pbrShader.setMat4("uProj",    proj);
    pbrShader.setVec3("uCamPos",  camPos);
    pbrShader.setVec3("uSunDir",  sunDir);
    pbrShader.setVec3("uSunColor",sunColor);
    pbrShader.setInt("uShadowMap", 0);
    pbrShader.setMat4("uLightMVP", lMat);

    // Track
    glm::mat4 trackModel = glm::mat4(1.0f);
    pbrShader.setMat4("uModel",   trackModel);
    pbrShader.setVec3("uAlbedo",  {0.15f,0.15f,0.15f});
    pbrShader.setFloat("uMetallic",   0.0f);
    pbrShader.setFloat("uRoughness",  0.9f);
    trackMesh.draw();

    // Car body
    glm::mat4 carModel = glm::mat4_cast(vs.orientation);
    carModel[3] = glm::vec4(vs.position, 1.0f);
    pbrShader.setMat4("uModel",  carModel);
    pbrShader.setVec3("uAlbedo", {0.88f,0.02f,0.02f}); // F1 red livery
    pbrShader.setFloat("uMetallic",  0.6f);
    pbrShader.setFloat("uRoughness", 0.3f);
    carMesh.draw();
}

void Renderer::drawMesh(const Mesh& mesh, const glm::vec3& albedo, float metallic, float roughness) {
    pbrShader.bind();
    pbrShader.setMat4("uModel", glm::mat4(1.0f));
    pbrShader.setVec3("uAlbedo", albedo);
    pbrShader.setFloat("uMetallic", metallic);
    pbrShader.setFloat("uRoughness", roughness);
    mesh.draw();
}

void Renderer::shutdown() {
    glDeleteFramebuffers(1, &shadowFBO);
    glDeleteTextures(1, &shadowTex);
}
