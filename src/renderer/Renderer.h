#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "../physics/VehicleState.h"
#include "../track/Track.h"

struct Renderer {
    Shader pbrShader;
    Shader shadowShader;

    GLuint shadowFBO  = 0;
    GLuint shadowTex  = 0;
    int    shadowSize = 2048;

    glm::vec3 sunDir;
    glm::vec3 sunColor = {1.4f, 1.2f, 1.0f};

    void init();
    void renderShadowPass(const Mesh& trackMesh, const glm::mat4& trackModel);
    void renderScene(const Camera& cam, const VehicleState& vs,
                     const Mesh& trackMesh, const Mesh& carMesh,
                     int screenW, int screenH);
    void drawMesh(const Mesh& mesh, const glm::vec3& albedo, float metallic, float roughness);
    void shutdown();

private:
    glm::mat4 lightSpaceMat() const;
};
