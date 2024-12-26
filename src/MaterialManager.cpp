#include "MaterialManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <vector>
#include "EngineState.h"

extern EngineState g_EngineState;
std::vector<Material> g_MaterialList;
std::map<std::string, int> g_MaterialName;

void MaterialManager::AddMaterial(Material& material, std::string name)
{
    g_MaterialList.push_back(material);
    g_MaterialName[name] = g_MaterialList.size() - 1;
    g_EngineState.MaterialUpdate = true;
}

void MaterialManager::SetMaterialName(Material& material, std::string name)
{
}

int MaterialManager::GetMaterialIndex(const Material& material)
{
    for (int i = 0; i <= g_MaterialList.size(); ++i) {
        if (material == g_MaterialList[i]) {
            return i;
        }
    }
}

void MaterialManager::OnImGuiRender()
{
    ImGui::Begin("Material Manager");
    for (auto& pair : g_MaterialName) {
        if (ImGui::Button(pair.first.c_str())) {
            ImGui::OpenPopup(pair.first.c_str());
        }
        if (ImGui::BeginPopup(pair.first.c_str())) {
            MaterialEditor materialEditor(g_MaterialList[pair.second]);
            materialEditor.OnImGuiRender();
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

MaterialManager g_MaterialManager;

MaterialEditor::MaterialEditor(Material& material)
    : m_Material(material)
{
}

void MaterialEditor::OnImGuiRender()
{
    ImGui::Text("Material Editor");
    Material t_Material = m_Material;
    ImGui::ColorEdit4("Ambient", &t_Material.Ambient.x);
    ImGui::ColorEdit4("Diffuse", &t_Material.Diffuse.x);
    ImGui::ColorEdit4("Specular", &t_Material.Specular.x);
    ImGui::ColorEdit4("Emission", &t_Material.Emission.x);
    ImGui::SliderFloat("Specular Exponent", &t_Material.SpecularExponent, 0.0f, 1000.0f);
    ImGui::SliderFloat("Dissolve", &t_Material.Dissolve, 0.0f, 1.0f);
    ImGui::SliderFloat("Optical Density", &t_Material.OpticalDensity, 0.0f, 10.0f);
    ImGui::SliderFloat("Roughness", &t_Material.Roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic", &t_Material.Metallic, 0.0f, 1.0f);
    if (!(t_Material == m_Material)) {
        g_EngineState.MaterialUpdate = true;
        m_Material = t_Material;
    }
}
