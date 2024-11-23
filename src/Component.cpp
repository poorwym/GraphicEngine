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
			v.Position = glm::vec3(globalTransform * glm::vec4(vertex.Position, 1.0f)); // ����任
            v.Normal = glm::normalize(glm::mat3(glm::transpose(glm::inverse(globalTransform))) * vertex.Normal);// ���߱任
			v.Tangent = glm::vec3(glm::mat3(globalTransform) * vertex.Tangent);
            v.Bitangent = glm::vec3(glm::mat3(globalTransform) * vertex.Bitangent);
			//vec3 transformedNormal = normalize(mat3(transpose(inverse(modelMatrix))) * normal);
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
