#include "LightController.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"



DirectionalLightController::DirectionalLightController()
	: LightController(nullptr)
{
}

DirectionalLightController::DirectionalLightController(DirectionalLight* selectedLight)
	: LightController(selectedLight)
{
	m_SelectedLight = static_cast<DirectionalLight *>(selectedLight);
}

const char* DirectionalLightController::GetImGuiTitle()
{
	const char* title = "Directional Light Controller";
	return title;
}

void DirectionalLightController::OnImGuiRender()
{
	DirectionalLight* selectedLight = static_cast<DirectionalLight*>(m_SelectedLight);
	this->OnImGuiRender(m_SelectedLight);
}

void DirectionalLightController::OnImGuiRender(DirectionalLight* selectedLight)
{
	ImGui::Text("Directional Light");
	static glm::vec3 color=selectedLight->GetLightColor();
	ImGui::ColorEdit3("Light Color", (float*)&color);
	selectedLight->SetLightColor(glm::vec3(color));
	static float intensity=selectedLight->GetLightIntensity();
	ImGui::SliderFloat("Intensity", &intensity, 0.0f, 1.0f);
	selectedLight->SetLightIntensity(intensity);
	static glm::vec3 dir=selectedLight->GetLightDir();
	ImGui::SliderFloat3("Direction", (float*)&dir, -1.0f, 1.0f);
	selectedLight->SetLightDir(dir);
}

void DirectionalLightController::Update(float deltaTime)
{
}

void DirectionalLightController::SelectedLight(Light* light)
{
	m_SelectedLight = static_cast<DirectionalLight*>(light);
}

LightController::LightController(Light* selectedLight)
{
	m_SelectedLight = selectedLight;
}
