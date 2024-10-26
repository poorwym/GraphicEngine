#include "Component.h"

Component::Component()
{
	m_LocalTransform = glm::mat4(1.0f);
}

MeshComponent::MeshComponent(Mesh* mesh)
	:m_Mesh(mesh)
{
	SetType();
}

void MeshComponent::Render(Shader& shader, Camera& camera, glm::mat4 globalTranform)
{
	glm::mat4 m_GlobalTransform = globalTranform * m_LocalTransform;
	m_Mesh->Render(shader, camera, m_GlobalTransform);
}

void MeshComponent::Update(float deltaTime)
{
}
