#include "EntityController.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

EntityController::EntityController(Entity* entity)
	: m_ControlledEntity(entity)
{

}

const char* EntityController::GetImGuiTitle()
{
	const char* title = "Entity Controller";
	return title;
}

void EntityController::Update(float deltaTime)
{
	m_ControlledEntity->Update(deltaTime);
}

void EntityController::TranslateEntity(const glm::vec3& direction)
{
	m_ControlledEntity->Translate(direction);
}

void EntityController::RotateEntity(const glm::vec3& rotation)
{
	m_ControlledEntity->Rotate(rotation);
}

void EntityController::ScaleEntity(const glm::vec3& scale)
{
	m_ControlledEntity->Scale(scale);
}

void EntityController::SelectEntity(Entity* entity)
{
	m_ControlledEntity = entity;
}

void EntityController::DeselectEntity()
{
	m_ControlledEntity = nullptr;
}

void EntityController::OnImGuiRender()
{
	if (m_ControlledEntity != nullptr)
    {
		ImGui::Text("Entity: %s", m_ControlledEntity->m_Name.c_str());
		// Translation
		glm::vec3 Position = m_ControlledEntity->GetPosition();
        ImGui::SliderFloat3("Translation", &(Position.x), -5.0f, 5.0f);
		m_ControlledEntity->SetPosition(Position);
		// Rotation
		glm::vec3 Rotation = m_ControlledEntity->m_Rotation;
        ImGui::SliderFloat3("Rotation", &(Rotation.x), -180.0f, 180.0f);
		m_ControlledEntity->m_Rotation = Rotation;
		// Scale
		float scale = m_ControlledEntity->m_Scale.x;
		ImGui::SliderFloat("Scale", &(scale), 0.1f, 5.0f);
		m_ControlledEntity->m_Scale = glm::vec3(scale);
    }
    else
    {
        ImGui::Text("No entity selected.");
    }
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
