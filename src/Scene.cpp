#include "Scene.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "LightController.h"
extern DirectionalLightController directionalLightController;

void Scene::load(const std::string& filePath)
{

}

void Scene::save(const std::string& filePath)
{

}

void Scene::Update(float deltaTime)
{
	for (auto& pair : m_SceneNodes)
	{
		SceneNode* node = pair.second;
		node->Update(deltaTime);
	}
}

void Scene::AddNode(SceneNode* node)
{
	m_SceneNodes[node->GetName()] = node;
}

void Scene::Render(Shader& shader, Camera& camera)
{
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		node->Render(shader, camera, glm::mat4(1.0f));
	}
}

void Scene::RemoveNode(SceneNode* node)
{
	m_SceneNodes[node->GetName()] = nullptr;
}

void Scene::OnImGuiTree()
{
	if (ImGui::TreeNode("Scene"))
	{
		if (ImGui::Button("DirectionalLight"))
		{
			ImGui::OpenPopup("DirectionalLightController");
		}
		if (ImGui::BeginPopup("DirectionalLightController"))
		{
			directionalLightController.OnImGuiRender();
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		for (auto& pair : m_SceneNodes) {
			pair.second->OnImGuiTree();
		}
		ImGui::TreePop();
	}

}
