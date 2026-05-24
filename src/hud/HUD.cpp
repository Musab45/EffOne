#include "HUD.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>

void HUD::init(GLFWwindow* win) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 430");
    ImGui::StyleColorsDark();
}

void HUD::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void HUD::drawTelemetry(const VehicleState& s) {
    if (!showTelemetry) return;
    ImGui::SetNextWindowPos({10,10});
    ImGui::SetNextWindowSize({320,400});
    ImGui::Begin("Telemetry", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    float speed = glm::length(s.velocity) * 3.6f; // km/h
    float Glat  = s.acceleration.x / 9.81f;
    float Glong = s.acceleration.z / 9.81f;

    ImGui::Text("Speed:    %.1f km/h", speed);
    ImGui::Text("Gear:     %d   RPM: %.0f", s.gear, s.rpm);
    ImGui::Text("Throttle: %.0f%%  Brake: %.0f%%", s.throttle*100, s.brake*100);
    ImGui::Text("Steer:    %.2f rad", s.steer);
    ImGui::Separator();
    ImGui::Text("G-lat:    %.2f G", Glat);
    ImGui::Text("G-long:   %.2f G", Glong);
    ImGui::Separator();
    ImGui::Text("Slip angle (deg): FL %.1f  FR %.1f", glm::degrees(s.slipAngle[0]), glm::degrees(s.slipAngle[1]));
    ImGui::Text("                  RL %.1f  RR %.1f", glm::degrees(s.slipAngle[2]), glm::degrees(s.slipAngle[3]));
    ImGui::Text("Slip ratio:       FL %.2f  FR %.2f", s.slipRatio[0], s.slipRatio[1]);
    ImGui::Text("                  RL %.2f  RR %.2f", s.slipRatio[2], s.slipRatio[3]);
    ImGui::Separator();
    ImGui::Text("Fz (N): FL %.0f  FR %.0f", s.fz[0], s.fz[1]);
    ImGui::Text("        RL %.0f  RR %.0f", s.fz[2], s.fz[3]);
    ImGui::Text("Susp (mm): FL %.1f  FR %.1f", s.suspensionCompression[0]*1000, s.suspensionCompression[1]*1000);
    ImGui::Text("           RL %.1f  RR %.1f", s.suspensionCompression[2]*1000, s.suspensionCompression[3]*1000);
    ImGui::End();
}

void HUD::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void HUD::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
