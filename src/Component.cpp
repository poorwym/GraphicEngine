#include "Component.h"

Component::Component()
{
	m_LocalTransform = glm::mat4(1.0f);
}



MeshComponent::MeshComponent()
{
	SetType();
}

MeshComponent::~MeshComponent()
{
	for (auto& mesh : m_Meshes) {
		delete mesh;
	}
}

void MeshComponent::AddMesh(Mesh* mesh)
{
	m_Meshes.push_back(mesh);
}

void MeshComponent::RenderDepthMap(Shader& shader, glm::mat4 globalTransform)
{
	glm::mat4 m_GlobalTransform = globalTransform * m_LocalTransform;
	for (auto& mesh : m_Meshes)
		mesh->RenderDepthMap(shader, globalTransform);
}

void MeshComponent::Render(Shader& shader, Camera& camera, glm::mat4 globalTransform)
{
	glm::mat4 m_GlobalTransform = globalTransform * m_LocalTransform;
	for (auto& mesh : m_Meshes)
		mesh->Render(shader, camera, m_GlobalTransform);
}

void MeshComponent::Update(float deltaTime)
{
}

std::vector<Vertex> MeshComponent::GetVertices(glm::mat4 globalTransform)
{
	std::vector<Vertex> vertices;
	for (auto& mesh : m_Meshes)
	{
		for (auto& vertex : mesh->GetVertices())
		{
			Vertex v;
			v.Position = glm::vec3(globalTransform * glm::vec4(vertex.Position, 1.0f));
			vertices.push_back(v);
		}
	}
    return vertices;
}

std::vector<unsigned int> MeshComponent::GetIndices()
{
	std::vector<unsigned int> indices;
    for (auto& mesh : m_Meshes)
    {
		std::vector<unsigned int> meshIndices = mesh->GetIndices();
        indices.insert(indices.end(), meshIndices.begin(), meshIndices.end());
    }
    return indices;
}
