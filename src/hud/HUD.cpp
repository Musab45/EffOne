#include "HUD.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <string>
#include <cstdio>

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

void HUD::drawRaceOverlay(const VehicleState& vs, float lapTime, float bestLap,
                           float sectorTime[3], float bestSector[3], int w, int h) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = {(float)w, (float)h};

    // Speed + gear block (bottom centre)
    ImGui::SetNextWindowPos({(float)w*0.5f - 80, (float)h - 90});
    ImGui::SetNextWindowSize({160, 80});
    ImGui::SetNextWindowBgAlpha(0.55f);
    ImGui::Begin("##speed", nullptr,
        ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoMove);
    float speed = glm::length(vs.velocity) * 3.6f;
    ImGui::SetWindowFontScale(2.3f);
    ImGui::Text("%3.0f", speed);
    ImGui::SameLine(); ImGui::SetWindowFontScale(1.08f); ImGui::Text("km/h");
    ImGui::SetWindowFontScale(1.7f);
    ImGui::Text("Gear %d", vs.gear);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::End();

    // RPM bar (bottom full width)
    ImGui::SetNextWindowPos({0, (float)h - 12});
    ImGui::SetNextWindowSize({(float)w, 12});
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##rpm", nullptr,
        ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoMove);
    float rpmFrac = (vs.rpm - 800.0f) / (15000.0f - 800.0f);
    ImVec4 col = (rpmFrac > 0.85f) ? ImVec4(1,0,0,1) : (rpmFrac > 0.6f) ? ImVec4(1,1,0,1) : ImVec4(0,1,0,1);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
    ImGui::ProgressBar(rpmFrac, {(float)w, 12}, "");
    ImGui::PopStyleColor();
    ImGui::End();

    // Lap time + sectors (top right)
    ImGui::SetNextWindowPos({(float)w - 200, 10});
    ImGui::SetNextWindowSize({190, 120});
    ImGui::SetNextWindowBgAlpha(0.55f);
    ImGui::Begin("##timing", nullptr,
        ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoMove);
    auto timeStr = [](float t) -> std::string {
        int m=(int)(t/60), s=(int)(t)%60; int ms=(int)((t-(int)t)*1000);
        char buf[16]; snprintf(buf,16,"%d:%02d.%03d",m,s,ms); return buf;
    };
    ImGui::Text("LAP  %s", timeStr(lapTime).c_str());
    ImGui::Text("BEST %s", bestLap > 0 ? timeStr(bestLap).c_str() : "--:--.---");
    ImGui::Separator();
    const char* labels[3] = {"S1","S2","S3"};
    for (int i = 0; i < 3; ++i) {
        bool pb = bestSector[i] > 0 && sectorTime[i] < bestSector[i];
        ImGui::TextColored(sectorTime[i] > 0 ? (pb ? ImVec4(0,1,0,1) : ImVec4(1,0.3f,0.3f,1))
                                              : ImVec4(0.6f,0.6f,0.6f,1),
                           "%s %s", labels[i], sectorTime[i] > 0 ? timeStr(sectorTime[i]).c_str() : "--:--.---");
    }
    ImGui::End();

    // DRS indicator (top left)
    if (vs.drsOpen) {
        ImGui::SetNextWindowPos({10, 10});
        ImGui::SetNextWindowSize({70, 30});
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin("##drs", nullptr,
            ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoMove);
        ImGui::TextColored({0,1,0.5f,1}, "DRS ON");
        ImGui::End();
    }
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
