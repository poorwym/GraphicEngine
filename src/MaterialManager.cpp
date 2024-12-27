#include "MaterialManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <vector>
#include "EngineState.h"

extern EngineState g_EngineState;
std::vector<PBRMaterial> g_MaterialList;

void MaterialManager::AddMaterial(PBRMaterial& material)
{
    g_MaterialList.push_back(material);
    g_EngineState.MaterialUpdate = true;
}


int MaterialManager::GetMaterialIndex(const PBRMaterial& material)
{
    for (int i = 0; i < g_MaterialList.size(); i++) {
        if (g_MaterialList[i].GetName() == material.GetName()) {
            return i;
        }
    }
    std::cout << "Material not found" << std::endl;
    return -1;
}

void MaterialManager::OnImGuiRender()
{
    ImGui::Begin("Material Manager");
    for (auto& m : g_MaterialList) {
        std::string name = m.GetName();
        if (ImGui::Button(name.c_str())) {
            ImGui::OpenPopup(name.c_str());
        }
        if (ImGui::BeginPopup(name.c_str())) {
            MaterialEditor materialEditor(m);
            materialEditor.OnImGuiRender();
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

MaterialManager g_MaterialManager;

MaterialEditor::MaterialEditor(PBRMaterial& material)
    : m_Material(material)
{
}

void MaterialEditor::OnImGuiRender()
{
    ImGui::Text("Material Editor");
    Material t_Material = m_Material.GetMaterial();
    ImGui::ColorEdit4("Ambient", &t_Material.Ambient.x);
    ImGui::ColorEdit4("Diffuse", &t_Material.Diffuse.x);
    ImGui::ColorEdit4("Specular", &t_Material.Specular.x);
    ImGui::ColorEdit4("Emission", &t_Material.Emission.x);
    ImGui::SliderFloat("Specular Exponent", &t_Material.SpecularExponent, 0.0f, 1000.0f);
    ImGui::SliderFloat("Dissolve", &t_Material.Dissolve, 0.0f, 1.0f);
    ImGui::SliderFloat("Optical Density", &t_Material.OpticalDensity, 0.0f, 10.0f);
    ImGui::SliderFloat("Roughness", &t_Material.Roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic", &t_Material.Metallic, 0.0f, 1.0f);
    if (!(t_Material == m_Material.GetMaterial())) {
        g_EngineState.MaterialUpdate = true;
        m_Material.SetMaterial(t_Material);
    }
}
