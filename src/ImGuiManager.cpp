#include "ImGuiManager.h" 
#include "imgui/imgui.h" 
#include "imgui/imgui_impl_glfw.h" 
#include "imgui/imgui_impl_opengl3.h" 
#include "glm/glm.hpp" 
#include "color.h" 
#include <iostream> 
#include <unordered_map> 

// 将颜色风格封装为一个映射表 
static std::unordered_map<std::string, std::tuple<glm::vec3, glm::vec3, glm::vec3>> buttonStyles = {
    {"red", {_RED, _BRIGHT_RED, _DARK_RED}},
    {"green", {_GREEN, _LIGHTGREEN, _DARKGREEN}},
    {"blue", {_BLUE, _LIGHTBLUE, _NAVY}},
    {"yellow", {_YELLOW, _BRIGHT_RED, _ORANGE}},
    {"purple", {_PURPLE, _LIGHTPURPLE, _PLUM}},
    {"cyan", {_CYAN, _LIGHTBLUE, _TURQUOISE}},
    {"pink", {_PINK, _BRIGHT_RED, _SALMON}},
    {"orange", {_ORANGE, _CORAL, _VERMILION}},
    {"grey", {_GREY, _LIGHTGREY, _DARKGREY}},
    {"gold", {_GOLD, _YELLOW, _ORANGE}},
    {"silver", {_SILVER, _LIGHTGREY, _WHITE}},
    {"brown", {_BROWN, _TAN, _SIENNA}}
};

static ImVec4 glmVec3_to_ImVec4(glm::vec3 vec) {
    return ImVec4(vec.x, vec.y, vec.z, 1.0f);
}

void ImGuiManager::SetButtonStyle(const char* style)
{
    std::string style_str(style);
    auto it = buttonStyles.find(style_str);
    if (it != buttonStyles.end()) {
        glm::vec3 normal, hovered, active;
        std::tie(normal, hovered, active) = it->second;
        ImGui::PushStyleColor(ImGuiCol_Button, glmVec3_to_ImVec4(normal));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, glmVec3_to_ImVec4(hovered));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, glmVec3_to_ImVec4(active));
    }
    else {
        std::cout << "Invalid button style: " << style << std::endl;
    }
}

void ImGuiManager::ResetButtonStyle()
{
    ImGui::PopStyleColor(3);
}

ImGuiManager g_ImGuiManager;


