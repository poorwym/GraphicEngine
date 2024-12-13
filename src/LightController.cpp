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
	ImGui::SliderFloat("Intensity", &intensity, 0.0f, 3.0f);
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

PointLightController::PointLightController()
	: LightController(nullptr)
{
}

PointLightController::PointLightController(PointLight* selectedLight)
	: LightController(selectedLight)
{
	m_SelectedLight =static_cast<PointLight*>(selectedLight);
}

void PointLightController::OnImGuiRender()
{
	PointLight* selectedLight = static_cast<PointLight*>(m_SelectedLight);
	this->OnImGuiRender(m_SelectedLight);
}

void PointLightController::OnImGuiRender(PointLight* selectedLight)
{
	ImGui::Text("Point Light");
	static glm::vec3 color = selectedLight->GetLightColor();
	ImGui::ColorEdit3("Light Color", (float*)&color);
	selectedLight->SetLightColor(glm::vec3(color));
	static float intensity = selectedLight->GetLightIntensity();
	ImGui::SliderFloat("Intensity", &intensity, 0.0f, 3.0f);
	selectedLight->SetLightIntensity(intensity);
	static glm::vec3 Position = selectedLight->GetLightPos();
	ImGui::SliderFloat3("Position", (float*)&Position, -1.0f, 1.0f);
	selectedLight->SetLightPos(Position);
	static float constant = selectedLight->GetConstant();
	ImGui::SliderFloat("Constant", &constant, 0.0f, 2.0f);
	selectedLight->SetConstant(constant);
	static float linear = selectedLight->GetLinear();
	ImGui::SliderFloat("Linear", &linear, 0.0f, 2.0f);
	selectedLight->SetLinear(linear);
	static float quadratic = selectedLight->GetQuadratic();
	ImGui::SliderFloat("Quadratic", &quadratic, 0.0f, 2.0f);
	selectedLight->SetQuadratic(quadratic);
}

void PointLightController::Update(float deltaTime)
{

}

void PointLightController::SelectedLight(Light* light)
{
	m_SelectedLight = static_cast<PointLight*>(light);
}

SpotLightController::SpotLightController()
	:LightController(nullptr)
{
}

SpotLightController::SpotLightController(SpotLight* selectedLight)
	:LightController(selectedLight)
{
    m_SelectedLight = static_cast<SpotLight*>(selectedLight);
}

void SpotLightController::OnImGuiRender()
{
    SpotLight* selectedLight = static_cast<SpotLight*>(m_SelectedLight);
    this->OnImGuiRender(m_SelectedLight);
}

void SpotLightController::OnImGuiRender(SpotLight* selectedLight)
{
    ImGui::Text("Spot Light");
    static glm::vec3 color = selectedLight->GetLightColor();
    ImGui::ColorEdit3("Light Color", (float*)&color);
    selectedLight->SetLightColor(glm::vec3(color));
    static float intensity = selectedLight->GetLightIntensity();
    ImGui::SliderFloat("Intensity", &intensity, 0.0f, 3.0f);
    selectedLight->SetLightIntensity(intensity);
    static glm::vec3 Position = selectedLight->GetLightPos();
    ImGui::SliderFloat3("Position", (float*)&Position, -1.0f, 1.0f);
}

void SpotLightController::Update(float deltaTime)
{

}

void SpotLightController::SelectedLight(Light* light)
{
    m_SelectedLight = static_cast<SpotLight*>(light);
}
