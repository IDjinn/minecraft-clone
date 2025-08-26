//
// Created by Luke on 26/08/2025.
//

#include "DebugGui.h"

#include <memory>

#include "Assert.h"
#include "imgui.h"
#include "../game/players/Player.h"
#include "../render/Render.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

void DebugGui::setup(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void DebugGui::prepare() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void DebugGui::render(const Render *render, const std::shared_ptr<Player> &player) {
    ImGui::Begin("Debug");
    ImGui::Text("FPS: %.1f", 1.0f / render->delta_time);
    ImGui::Text("Pos: (%.1f, %.1f, %.1f)", player->position.x, player->position.y, player->position.z);
    ImGui::Text("Yaw: %.1f, Pitch: %.1f", render->yaw, render->pitch);
    ImGui::Text("Mouse: %s", render->mouseEnabled ? "Enabled" : "Disabled");
    ImGui::Text("Press ESC to toggle mouse"); {
        ImGui::BeginChild("Console", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false,
                          ImGuiWindowFlags_HorizontalScrollbar);
        for (auto &line: debug_output)
            ImGui::TextUnformatted(line.c_str());
        ImGui::EndChild();
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugGui::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
