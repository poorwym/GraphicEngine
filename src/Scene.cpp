#include "Scene.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "LightController.h"
#include "color.h"
#include "SceneManager.h"
extern DirectionalLightController directionalLightController;
static void BatchBindTextures(Shader& shader) {
	shader.Bind();
	for (auto& texture : textureList) {
		unsigned int slot = textureSlots[texture->GetTextureID()];
		std::string uniformName = "textures["+std::to_string(slot) + "]";
		shader.setUniform1i(uniformName.c_str(), slot);
		texture->Bind(slot);
	}
	shader.Unbind();
}

void Scene::UpdateVAO() {
	std::vector<Vertex> batchVertices;
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		std::vector<Vertex> nodeVertices = node->GetVertices(glm::mat4(1.0f));
		batchVertices.insert(batchVertices.end(), nodeVertices.begin(), nodeVertices.end());
	}
	std::vector<unsigned int> batchIndices;
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		std::vector<unsigned int> nodeIndices = node->GetIndices();
		batchIndices.insert(batchIndices.end(), nodeIndices.begin(), nodeIndices.end());
	}

	VertexBufferLayout layout;
	layout.Push<float>(3); // Position: 3个浮点数
	layout.Push<float>(3); // Normal: 3个浮点数
	layout.Push<float>(2); // TexCoords: 2个浮点数
	layout.Push<float>(3); // 切线
	layout.Push<float>(3); // 双切线
	layout.Push<float>(7); //textureSlot

	m_VAO = new VertexArray();
	m_VBO = new VertexBuffer(batchVertices.data(), batchVertices.size() * sizeof(Vertex));
	m_VAO->AddBuffer(*m_VBO, layout);
	m_IBO = new IndexBuffer(batchIndices.data(), batchIndices.size());
}
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
static void PlanetRotate(SceneNode* node, float rate, float deltaTime, float& angle) {
	angle += rate * deltaTime;
	if (angle > 180) {
		angle -= 360;
	}
	node->SetRotation(glm::vec3(0.0f, angle, 0.0f));
}
static void StaticUpdate(float deltaTime) {
	return;
}
void Scene::Update(float deltaTime)
{
	m_DirLight->Update(deltaTime);
	StaticUpdate(deltaTime);
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
	shader.setUniformMat4f("lightSpaceMatrix", m_DirLight->ComputeLightSpaceMatrix(glm::vec3(0.0f)));
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		node->BindLight(shader, globalTransform);
	}
}

void Scene::RenderDepthMap(Shader& shader)
{
	Renderer renderer;
	renderer.Draw(*m_VAO, *m_IBO, shader);
}

void Scene::RenderShadowMap(Shader* depthShader, Shader* cubeDepthShader)
{
	UpdateVAO();
	depthShader->Bind();
	glm::mat4 lightSpaceMatrix = m_DirLight->ComputeLightSpaceMatrix(glm::vec3(0.0f));
	depthShader->setUniformMat4f("SpaceMatrix", lightSpaceMatrix);
	m_DirLight->m_ShadowMapFBO->Bind();
	RenderDepthMap(*depthShader);
	depthShader->Unbind();
	m_DirLight->m_ShadowMapFBO->Unbind();
	//bind shadowMap
	m_DirLight->m_ShadowMapFBO->BindTexture(31);

	int count = 0;
	for(auto& pair: pointLightList)
	{
		PointLight* pointlight = pair.second;
		std::vector<glm::mat4> shadowMatrices = pointlight->ComputePointLightShadowMatrices(NEAR_PLANE, FAR_PLANE);
		pointlight->m_CubeShadowMapFBO->Bind();
		cubeDepthShader->Bind();
		cubeDepthShader->setUniform1f("farPlane", FAR_PLANE);
		cubeDepthShader->setUniformVec3f("lightPos", pointlight->GetLightPos());
		for (unsigned int i = 0; i < 6; ++i) {
			cubeDepthShader->setUniformMat4f("ShadowMatrices[" + std::to_string(i) + "]", shadowMatrices[i]);
			pointlight->m_CubeShadowMapFBO->BindFace(i);
			glClear(GL_DEPTH_BUFFER_BIT);
			RenderDepthMap(*cubeDepthShader);
		}
		cubeDepthShader->Unbind();
		pointlight->m_CubeShadowMapFBO->Unbind();
		pointlight->m_CubeShadowMapFBO->BindTexture(27 + count);
		count++;
	}
}

void Scene::BatchRender(Shader& shader, Camera& camera)
{
	BatchBindTextures(shader);
	Renderer render;
	glm::mat4 lightSpaceMatrix = m_DirLight->ComputeLightSpaceMatrix(glm::vec3(0.0f));
	render.Draw(*m_VAO, *m_IBO, &camera, shader, glm::mat4(1.0f), &lightSpaceMatrix);
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
