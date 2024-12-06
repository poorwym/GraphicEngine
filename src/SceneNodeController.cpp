#include "SceneNodeController.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SceneManager.h"

SceneNodeController::SceneNodeController(SceneNode* sceneNode)
{
	m_ControlledSceneNode = sceneNode;
}

void SceneNodeController::Update(float deltaTime)
{
}

void SceneNodeController::TranslateSceneNode(const glm::vec3& direction)
{
}

void SceneNodeController::RotateSceneNode(const glm::vec3& rotation)
{
}

void SceneNodeController::SelectSceneNode(SceneNode* sceneNode)
{
	m_ControlledSceneNode = sceneNode;
}

void SceneNodeController::DeselectSceneNode()
{
	m_ControlledSceneNode = nullptr;
}

void SceneNodeController::OnImGuiRender()
{
	if (m_ControlledSceneNode != nullptr)
	{
		ImGui::Text("SceneNode: %s", m_ControlledSceneNode->GetName().c_str());
		glm::vec3 Position = m_ControlledSceneNode->GetPosition();
		ImGui::SliderFloat3("Position", &(Position.x), -5.0f, 5.0f);
		m_ControlledSceneNode->SetPosition(Position);
		glm::vec3 Rotation = m_ControlledSceneNode->GetRotation();
		ImGui::SliderFloat3("Rotation", &(Rotation.x), -180.0f, 180.0f);
		m_ControlledSceneNode->SetRotation(Rotation);
	}
	else
	{
		ImGui::Text("No sceneNode selected.");
	}
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
