#include "LightManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Light.h"
#include "color.h"
#include "SceneManager.h"
#include "Macro.h"

extern SceneManager g_SceneManager;

std::map<std::string, PointLight*> g_PointLightList;
std::map<PointLight*, unsigned int> g_PointLightID;
std::map<std::string, SpotLight*> g_SpotLightList;
std::map<SpotLight*, unsigned int> g_SpotLightID;
std::map<std::string, DirectionalLight*> g_DirectionalLightList;
std::map<DirectionalLight*, unsigned int> g_DirectionalLightID;

void LightManager::OnImGuiRender()
{
	ImGui::Begin("Light Manager");
	ImGui::Text("Add Directional Light");
	if (ImGui::Button("Add Directional Light"))
	{
		AddDirectionalLight();
	}
	ImGui::Text("Add Point Light");
    if (ImGui::Button("Add Point Light"))
    {
        AddPointLight();
    }
    ImGui::Text("Add Spot Light");
    if (ImGui::Button("Add Spot Light"))
    {
        AddSpotLight();
    }
	ImGui::End();
}

void LightManager::AddDirectionalLight()
{
	if (g_DirectionalLightList.size() >= MAX_DIRECTIONAL_LIGHTS) {
		std::cout << "Max Directional Light Reached" << std::endl;
		return;
	}
	int ID = g_DirectionalLightList.size();
	std::string name = "Directional Light" + std::to_string(ID);
	DirectionalLight* light = new DirectionalLight(name, _DARKGREY, 3.0f, glm::vec3(1.0f));
	g_SceneManager.AddDirectionalLight(light, name.c_str(), nullptr);
}

void LightManager::AddPointLight()
{
    if (g_PointLightList.size() >= MAX_POINT_LIGHTS) {
        std::cout << "Max Point Light Reached" << std::endl;
        return;
    }
    int ID = g_PointLightList.size();
    std::string name = "Point Light" + std::to_string(ID);
    PointLight* light = new PointLight(name, _DARKGREY, 3.0f, glm::vec3(1.0f));
    g_SceneManager.AddPointLight(light, name.c_str(), nullptr);
}

void LightManager::AddSpotLight()
{
    if (g_SpotLightList.size() >= MAX_SPOT_LIGHTS) {
        std::cout << "Max Spot Light Reached" << std::endl;
        return;
    }
    int ID = g_SpotLightList.size();
    std::string name = "Spot Light" + std::to_string(ID);
    //SpotLight* light = new SpotLight(name, _DARKGREY, 3.0f, glm::vec3(1.0f));
    //g_SceneManager.AddSpotLight(light, name.c_str(), nullptr);
}

LightManager g_LightManager;
