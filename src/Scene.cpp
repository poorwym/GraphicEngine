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
	shader.setUniform1i("textures", 0);
	textureArray->Bind();
	shader.Unbind();
}

void Scene::UpdateVAO() {
	std::vector<std::vector<Vertex>*> batchVertices;
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		std::vector<std::vector<Vertex>*> nodeVertices = node->GetVertices(glm::mat4(1.0f));
		batchVertices.insert(batchVertices.end(), nodeVertices.begin(), nodeVertices.end());
	}
	std::vector<std::vector<unsigned int>*> batchIndices;
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		std::vector<std::vector<unsigned int>*> nodeIndices = node->GetIndices();
		batchIndices.insert(batchIndices.end(), nodeIndices.begin(), nodeIndices.end());
	}
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int offset = 0;
	for (int i = 0; i < batchIndices.size(); i++) {
		std::vector<Vertex> t_vertices = *(batchVertices[i]);
		std::vector<unsigned int> t_indices = *(batchIndices[i]);
		vertices.insert(vertices.end(), t_vertices.begin(), t_vertices.end());
		for (auto& index : t_indices) {
            index += offset;
			indices.push_back(index);
		}
        offset += t_vertices.size();
		delete batchVertices[i];
        delete batchIndices[i];
	}
	VertexBufferLayout layout;
	layout.Push<float>(3); // Position: 3个浮点数
	layout.Push<float>(3); // Normal: 3个浮点数
	layout.Push<float>(2); // TexCoords: 2个浮点数
	layout.Push<float>(1); //textureSlot
	layout.Push<float>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);
	m_Vertices.clear();
	m_Indices.clear();
	m_Vertices = vertices;
    m_Indices = indices;

	m_VAO = new VertexArray();
	m_VBO = new VertexBuffer(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
	m_VAO->AddBuffer(*m_VBO, layout);
	m_IBO = new IndexBuffer(m_Indices.data(), m_Indices.size());

	m_VAO->Unbind();
	m_IBO->Unbind();
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
	renderer.Draw(*m_VAO, *m_IBO, nullptr, shader, glm::mat4(1.0f));
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
