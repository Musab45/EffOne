#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "renderer/Shader.h"
#include "renderer/Mesh.h"
#include "game/GameLoop.h"
#include "input/InputManager.h"
#include "physics/VehicleState.h"
#include "physics/RigidBody.h"
#include <functional>

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win = glfwCreateWindow(1280, 720, "F1 Sim", nullptr, nullptr);
    if (!win) { std::cerr << "Failed to create GLFW window\n"; glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1); // enable VSync
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n"; return 1;
    }
    glEnable(GL_DEPTH_TEST);

    // NOTE: exe must be run from project root so relative shader paths resolve correctly
    Shader shader;
    shader.load("assets/shaders/simple.vert", "assets/shaders/simple.frag");

    Mesh ground = Mesh::makeFlat(50.0f, 20);

    glm::mat4 proj  = glm::perspective(glm::radians(60.0f), 1280.0f/720.0f, 0.1f, 1000.0f);
    glm::mat4 model = glm::mat4(1.0f);

    InputManager input;
    input.init(win);

    VehicleState vs;
    RigidBody    rb;

    GameLoop loop;
    loop.run(
        [&](double dt) {
            InputState in = input.poll();
            if (in.quit || glfwWindowShouldClose(win)) { loop.shouldQuit = true; return; }

            rb.reset();
            glm::mat3 R = glm::mat3_cast(vs.orientation);
            glm::vec3 forward = R * glm::vec3(0,0,-1); // local -Z is forward

            // Apply 5000N forward force when throttle held (crude test)
            rb.applyForce(forward * in.throttle * 5000.0f);
            // Gravity
            rb.applyForce({0, -9.81f * RigidBody::MASS, 0});
            rb.integrate(vs, (float)dt);

            // Ground plane clamp (temporary until wheel raycasts in Task 8)
            if (vs.position.y < 0.3f) { vs.position.y = 0.3f; vs.velocity.y = 0.0f; }

            // Store inputs in state
            vs.throttle = in.throttle;
            vs.brake    = in.brake;
            vs.steer    = in.steer;
        },
        [&](double alpha) {
            glClearColor(0.05f, 0.05f, 0.05f, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Camera follows car
            glm::vec3 eye = vs.position + glm::vec3(0, 3, 8);
            glm::mat4 view = glm::lookAt(eye, vs.position, glm::vec3(0,1,0));
            glm::mat4 mvp  = proj * view * model;

            shader.bind();
            shader.setMat4("uMVP", mvp);
            ground.draw();
            glfwSwapBuffers(win);
        }
    );

    ground.free();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
