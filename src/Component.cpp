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
}

void MeshComponent::Render(Shader& shader, Camera& camera, glm::mat4 globalTransform)
{
	glm::mat4 m_GlobalTransform = globalTransform * m_LocalTransform;
}

void MeshComponent::Update(float deltaTime)
{
}

std::vector<std::vector<Vertex>*> MeshComponent::GetVertices(glm::mat4 globalTransform)
{
	std::vector<std::vector<Vertex>*> vertices;
	for (auto& mesh : m_Meshes)
	{
		std::vector <Vertex> *meshVertices = new std::vector <Vertex>;
		for (auto& vertex : mesh->GetVertices())
		{
			Vertex v = vertex;
			v.Position = glm::vec3(globalTransform * glm::vec4(vertex.Position, 1.0f));
			meshVertices->push_back(v);
		}
        vertices.push_back(meshVertices);
	}
    return vertices;
}

std::vector<std::vector<unsigned int>*> MeshComponent::GetIndices()
{
	std::vector<std::vector<unsigned int>*> indices;
    for (auto& mesh : m_Meshes)
    {
		std::vector<unsigned int> *meshIndices = new std::vector<unsigned int>(mesh->GetIndices());
		indices.push_back(meshIndices);
    }
    return indices;
}
