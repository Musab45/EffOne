#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <cmath>
#include "renderer/Shader.h"
#include "renderer/Mesh.h"
#include "game/GameLoop.h"
#include "input/InputManager.h"
#include "physics/VehicleState.h"
#include "physics/RigidBody.h"
#include "physics/TireModel.h"
#include "physics/Suspension.h"
#include "physics/Aerodynamics.h"
#include "physics/Powertrain.h"
#include "renderer/Camera.h"
#include "renderer/Renderer.h"
#include "renderer/PostFX.h"
#include "hud/HUD.h"
#include "track/Track.h"
#include "track/TrackCollision.h"
#include "audio/AudioEngine.h"
#include "game/LapTimer.h"
#include <functional>

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win = glfwCreateWindow(1280, 720, "F1 Sim", nullptr, nullptr);
    if (!win) { std::cerr << "Failed to create GLFW window\n"; glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n"; return 1;
    }
    glEnable(GL_DEPTH_TEST);

    Mesh ground = Mesh::makeFlat(2000.0f, 100);

    InputManager input;
    input.init(win);

    HUD hud;
    hud.init(win);

    Renderer renderer;
    renderer.init();
    Mesh carMesh = Mesh::makeBox(0.5f, 0.2f, 2.25f);

    PostFX postfx;
    postfx.init(1280, 720);

    AudioEngine audio; audio.init();

    VehicleState vs;
    RigidBody    rb;
    TireModel    tire;
    Suspension   susp;
    Aerodynamics aero;
    Powertrain   pt;
    Camera       cam;

    Track          track;
    TrackCollision collision;
    track.loadSpa("assets/track/spa.spa");
    collision.init(&track);

    LapTimer lapTimer;

    GameLoop loop;
    loop.run(
        [&](double dt) {
            InputState in = input.poll();
            if (in.quit || glfwWindowShouldClose(win)) { loop.shouldQuit = true; return; }

            // Update inputs in state
            vs.throttle = in.throttle;
            vs.brake    = in.brake;
            // Speed-sensitive steering: reduce ratio at high speed
            float speed   = glm::length(vs.velocity);
            float steerMax = glm::radians(18.0f) / (1.0f + speed * 0.03f);
            vs.steer = in.steer * steerMax;
            if (in.gearUp   && vs.gear < 8) vs.gear++;
            if (in.gearDown && vs.gear > 1) vs.gear--;
            if (in.drs) vs.drsOpen = !vs.drsOpen;
            if (in.toggleTelemetry) hud.showTelemetry = !hud.showTelemetry;

            // 1. Powertrain (updates rpm, wheelSpeed, slipRatio)
            pt.update(vs, (float)dt);

            // 2. Aero
            float downforce    = aero.computeDownforce(vs);
            glm::vec3 drag     = aero.computeDrag(vs);

            // 3. Suspension loads (static + weight transfer)
            susp.computeLoads(vs, vs.acceleration);
            // Add aero downforce equally across axles
            for (int i = 0; i < 4; ++i) vs.fz[i] += downforce * 0.25f;

            // 4. Forces
            rb.reset();
            rb.applyForce({0, -9.81f * RigidBody::MASS, 0}); // gravity
            rb.applyForce(drag);

            glm::mat3 R = glm::mat3_cast(vs.orientation);
            glm::vec3 forward = R * glm::vec3(0,0,-1);
            glm::vec3 right   = R * glm::vec3(1,0,0);

            // Per-wheel longitudinal + lateral tire forces (Pacejka via TireModel).
            // Drive torque is delivered through wheelSpeed (Powertrain::update)
            // so longitudinal force comes purely from slip ratio. Front wheels
            // steer, so their forward/right directions follow the steer angle.
            float carSpeed = glm::length(vs.velocity);
            for (int i = 0; i < 4; ++i) {
                glm::vec3 wheelFwd   = forward;
                glm::vec3 wheelRight = right;
                if (i < 2) { // front wheels steer
                    glm::mat4 steerRot = glm::rotate(glm::mat4(1.0f), vs.steer, glm::vec3(0,1,0));
                    wheelFwd   = glm::normalize(glm::vec3(steerRot * glm::vec4(forward, 0.0f)));
                    wheelRight = glm::normalize(glm::vec3(steerRot * glm::vec4(right,   0.0f)));
                }

                // Longitudinal slip + force
                float wv    = vs.wheelSpeed[i] * Powertrain::WHEEL_RADIUS;
                float denom = std::max(carSpeed, 0.5f);
                vs.slipRatio[i] = (wv - carSpeed) / denom;
                float longF = tire.longitudinalForce(vs.slipRatio[i], vs.fz[i]);
                rb.applyForceAtPoint(wheelFwd * longF, R * WHEEL_OFFSETS[i]);

                // Lateral slip + force (along wheel's steered lateral axis)
                float vLat  = glm::dot(vs.velocity, wheelRight);
                float vLong = glm::dot(vs.velocity, wheelFwd);
                vs.slipAngle[i] = (std::abs(vLong) > 0.1f) ? std::atan2(vLat, std::abs(vLong)) : 0.0f;
                float latF  = tire.lateralForce(vs.slipAngle[i], vs.fz[i]);
                glm::vec3 latDir = -wheelRight * (vs.slipAngle[i] >= 0.0f ? 1.0f : -1.0f);
                rb.applyForceAtPoint(latDir * std::abs(latF), R * WHEEL_OFFSETS[i]);
            }

            rb.integrate(vs, (float)dt);

            collision.resolveWheels(vs, rb, susp.springRate, susp.restLength);
            cam.update(vs, (float)dt);
            float progress = track.lapProgress(vs.position);
            bool newLap = lapTimer.update(progress, (float)dt);
            (void)newLap; // can flash HUD later
            audio.update(vs);
        },
        [&](double /*alpha*/) {
            // Drain GLFW event queue once per rendered frame. Doing this here
            // (instead of inside InputManager::poll, which only runs on physics
            // ticks) guarantees the window stays responsive even when the
            // accumulator hasn't built up a tick.
            glfwPollEvents();
            glm::mat4 trackModel = glm::mat4(1.0f);
            renderer.renderShadowPass(ground, trackModel);
            postfx.bindHDR();
            renderer.renderScene(cam, vs, ground, carMesh, 1280, 720);
            float speed = glm::length(vs.velocity) * 3.6f;
            postfx.apply(vs, speed);
            hud.beginFrame();
            hud.drawRaceOverlay(vs, lapTimer.lapTime, lapTimer.bestLap,
                    lapTimer.sectorTime, lapTimer.bestSector, 1280, 720);
            hud.drawTelemetry(vs);
            hud.endFrame();
            glfwSwapBuffers(win);
        }
    );

    audio.shutdown();
    postfx.shutdown();
    hud.shutdown();
    renderer.shutdown();
    carMesh.free();
    ground.free();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
