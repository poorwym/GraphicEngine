#include "Scene.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "LightController.h"
#include "color.h"
#include "SceneManager.h"
extern DirectionalLightController directionalLightController;

Scene::Scene()
	:m_DirLight(nullptr)
{
}

void Scene::load(const std::string& filePath)
{

}

void Scene::save(const std::string& filePath)
{

}

void Scene::Update(float deltaTime)
{
	m_DirLight->Update(deltaTime);
	//earth
	SceneNode* node_earth = sceneNodeList["node2"];
	static float angleEarth = 0.0f;
	angleEarth += 50 * deltaTime;
	if (angleEarth > 180) {
		angleEarth -= 360;
	}
	node_earth->SetRotation(glm::vec3(0.0f, angleEarth, 0.0f));
	//moon
	SceneNode* node_Moon = sceneNodeList["node3"];
	
	static float angleMoon = 0.0f;

	angleMoon += 300 * deltaTime;
	if (angleMoon > 180) {
		angleMoon -= 360;
	}
	node_Moon->SetRotation(glm::vec3(0.0f, angleMoon, 0.0f));

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

void Scene::SetDirectionalLight(DirectionalLight* dirLight)
{
	m_DirLight = dirLight;
}

void Scene::BindLight(Shader& shader, glm::mat4 globalTransform)
{
	m_DirLight->Bind(shader, globalTransform);
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		node->BindLight(shader, globalTransform);
	}
}

void Scene::RenderDepthMap(Shader& shader)
{
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		node->RenderDepthMap(shader, glm::mat4(1.0f));
	}
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
