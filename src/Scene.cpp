#include "Scene.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "LightController.h"
#include "color.h"
#include "SceneManager.h"
#include "Quad.h"
#include "EngineState.h"


extern DirectionalLightController directionalLightController;

static void BatchBindTextures(Shader& shader) {
	shader.Bind();
	shader.SetUniform1i("textures", 0);
	g_TextureArray->Bind();
	shader.Unbind();
}

void Scene::UpdateVBO() {
	m_VBO->Bind();
	m_VBO->setData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
    m_VBO->Unbind();
}
extern EngineState engineState;

void Scene::UpdateVertices()
{
	if (!engineState.needUpdate) {
		return;
	}
	int offsets = 0;
	std::vector<std::vector<Vertex>*> batchVertices;
	std::vector<std::vector<unsigned int>*> batchIndices;
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		std::vector<std::vector<Vertex>*> nodeVertices = node->GetVertices(glm::mat4(1.0f));
        std::vector<std::vector<unsigned int>*> nodeIndices = node->GetIndices();
        batchIndices.insert(batchIndices.end(), nodeIndices.begin(), nodeIndices.end());
		batchVertices.insert(batchVertices.end(), nodeVertices.begin(), nodeVertices.end());
	}
	m_Vertices.clear();
	m_Vertices.reserve(numVertices);
	for (int i = 0; i < batchVertices.size(); i++) {
		std::vector<Vertex>& vertices = *batchVertices[i];
		m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
		if(m_Indices.size() != numIndices)
		{
			std::vector<unsigned int>& indices = *batchIndices[i];
			for (auto& index : indices) {
				m_Indices.push_back(index + offsets);
			}
			offsets += vertices.size();
		}
	}
    numVertices = m_Vertices.size();
	numIndices = m_Indices.size();
	engineState.needUpdate = false;
}
void Scene::FreeVAO()
{
	delete m_VAO;
	delete m_VBO;
	delete m_IBO;
}
void Scene::ResetVAO()
{
	VertexBufferLayout layout;
	layout.Push<float>(4); // Position: 3个浮点数
	layout.Push<float>(4); // Normal: 3个浮点数
	layout.Push<float>(4); // TexCoords: 2个浮点数
	layout.Push<float>(4); // Tangent: 3个浮点数
	layout.Push<float>(4); // Bitangent: 3个浮点数
	layout.Push<float>(1); //textureSlot
	layout.Push<float>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);

	layout.AddStride(24 * sizeof(float));
	UpdateVertices();
	m_VAO = new VertexArray();
	m_VBO = new VertexBuffer(nullptr, m_Vertices.size() * sizeof(Vertex));
	UpdateVBO();
    m_IBO = new IndexBuffer(m_Indices.data(), m_Indices.size());

	m_VAO->Bind();
    m_VAO->AddBuffer(*m_VBO, layout);
    m_VAO->Unbind();
}
Scene::Scene(int n)
	:m_DirLight(nullptr),m_VAO(nullptr),m_VBO(nullptr),m_IBO(nullptr),m_TileQuad(n),numVertices(1),numIndices(1)
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
	shader.Bind();
	m_DirLight->Bind(shader, globalTransform);
	shader.SetUniform1i("numPointLights", g_PointLightID.size());
	shader.SetUniformMat4f("lightSpaceMatrix", m_DirLight->ComputeLightSpaceMatrix(glm::vec3(0.0f)));
	for (auto& pair : m_SceneNodes) {
		SceneNode* node = pair.second;
		node->BindLight(shader, globalTransform);
	}
	shader.Unbind();
}

void Scene::RenderDepthMap(Shader& shader) const
{
	Renderer renderer;
	renderer.Draw(*m_VAO, *m_IBO, nullptr, shader, glm::mat4(1.0f));
}

void Scene::RenderShadowMap(Shader* depthShader, Shader* cubeDepthShader)
{
	UpdateVertices();
	UpdateVBO();
	depthShader->Bind();
	glm::mat4 lightSpaceMatrix = m_DirLight->ComputeLightSpaceMatrix(glm::vec3(0.0f));
	depthShader->SetUniformMat4f("SpaceMatrix", lightSpaceMatrix);
	m_DirLight->m_ShadowMapFBO->Bind();
	RenderDepthMap(*depthShader);
	depthShader->Unbind();
	m_DirLight->m_ShadowMapFBO->Unbind();
	//bind shadowMap
	m_DirLight->m_ShadowMapFBO->BindTexture(31);

	int count = 0;
	for(auto& pair: g_PointLightList)
	{
		PointLight* pointlight = pair.second;
		std::vector<glm::mat4> shadowMatrices = pointlight->ComputePointLightShadowMatrices(NEAR_PLANE, FAR_PLANE);
		pointlight->m_CubeShadowMapFBO->Bind();
		cubeDepthShader->Bind();
		cubeDepthShader->SetUniform1f("farPlane", FAR_PLANE);
		cubeDepthShader->SetUniformVec3f("lightPos", pointlight->GetLightPos());
		for (unsigned int i = 0; i < 6; ++i) {
			cubeDepthShader->SetUniformMat4f("ShadowMatrices[" + std::to_string(i) + "]", shadowMatrices[i]);
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
	BindLight(shader, glm::mat4(1.0f));
	Renderer render;
	glm::mat4 lightSpaceMatrix = m_DirLight->ComputeLightSpaceMatrix(glm::vec3(0.0f));
	render.Draw(*m_VAO, *m_IBO, &camera, shader, glm::mat4(1.0f), &lightSpaceMatrix);
	g_TextureArray->Unbind();
}

void Scene::RayTracingRender(Shader& shader, Camera& camera, GLFWwindow* window)
{
	BatchBindTextures(shader);

	shader.Bind();
	shader.SetUniformVec3f("cameraPosition", camera.GetPosition());
    shader.SetUniform1f("nearPlane", camera.GetNearClip());
    shader.SetUniform1i("screenWidth", WINDOW_WIDTH);
    shader.SetUniform1i("screenHeight", WINDOW_HEIGHT);
    shader.SetUniform1f("fov", camera.GetFov());
    shader.SetUniform1f("aspectRatio", camera.GetAspectRatio());
	shader.SetUniformMat4f("viewMatrix", camera.GetViewMatrix());
    shader.SetUniform1f("seed", glfwGetTime());
	shader.Unbind();

	BindLight(shader, glm::mat4(1.0f));

    m_TileQuad.Render(shader, window);
	g_TextureArray->Unbind();
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
