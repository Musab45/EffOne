#include "InputManager.h"

void InputManager::init(GLFWwindow* window) { win = window; }

InputState InputManager::poll() {
    glfwPollEvents();
    InputState s;
    s.quit     = glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    s.throttle = (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS ||
                  glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS) ? 1.0f : 0.0f;
    s.brake    = (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS ||
                  glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS) ? 1.0f : 0.0f;
    if      (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS ||
             glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS)  s.steer = -1.0f;
    else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS ||
             glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS) s.steer =  1.0f;

    bool shift = glfwGetKey(win, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS;
    bool ctrl  = glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    bool space = glfwGetKey(win, GLFW_KEY_SPACE)         == GLFW_PRESS;
    bool r     = glfwGetKey(win, GLFW_KEY_R)             == GLFW_PRESS;
    bool f1    = glfwGetKey(win, GLFW_KEY_F1)            == GLFW_PRESS;

    s.gearUp   = shift && !prevShift;
    s.gearDown = ctrl  && !prevCtrl;
    s.drs      = space && !prevSpace;
    s.reset    = r     && !prevR;
    s.toggleTelemetry = f1 && !prevF1;

    prevShift = shift; prevCtrl = ctrl; prevSpace = space;
    prevR = r; prevF1 = f1;
    return s;
}
