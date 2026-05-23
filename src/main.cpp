#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "renderer/Shader.h"
#include "renderer/Mesh.h"
#include "game/GameLoop.h"
#include "input/InputManager.h"
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
    glm::mat4 view  = glm::lookAt(glm::vec3(0,10,20), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 model = glm::mat4(1.0f);

    InputManager input;
    input.init(win);

    GameLoop loop;
    loop.run(
        [&](double dt) {
            InputState s = input.poll();
            if (s.quit || glfwWindowShouldClose(win)) { loop.shouldQuit = true; return; }
            // physics placeholder: throttle/brake/steer captured but car not yet simulated
        },
        [&](double alpha) {
            glClearColor(0.05f, 0.05f, 0.05f, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.bind();
            shader.setMat4("uMVP", proj * view * model);
            ground.draw();
            glfwSwapBuffers(win);
        }
    );

    ground.free();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
