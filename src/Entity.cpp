#include "Entity.h"

Entity::Entity(std::string name)
	:m_Position(glm::vec3(0.0f)),m_Rotation(glm::vec3(0.0f)), m_Scale(glm::vec3(1.0f)),m_Name(name),m_LocalTransform(glm::mat4(1.0f))
{
}

Entity::~Entity()
{
	for (auto& pair : m_Components) {
		delete pair.second; // 释放每个组件
	}
}

void Entity::AddComponent(Component* component)
{
	m_Components[m_Components.size()] = component;
}

void Entity::RemoveComponent(Component* component)
{
	for (auto& pair : m_Components) {
		if (pair.second == component) {
			pair.second = nullptr;
		}
	}
}

void Entity::SetPosition(glm::vec3 position)
{
	m_Position = position;
}

void Entity::SetRotation(glm::vec3 rotation)
{
	m_Rotation = rotation;
}

void Entity::SetScale(glm::vec3 scale)
{
	m_Scale = scale;
}

void Entity::Rotate(glm::vec3 rotation)
{
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Entity::Translate(glm::vec3 translation)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation);
	m_LocalTransform = transform * m_LocalTransform;
}

void Entity::Scale(glm::vec3 scale)
{
	glm::mat4 transform = glm::scale(glm::mat4(1.0f), scale);
	m_LocalTransform = transform * m_LocalTransform;
}

void Entity::RenderDepthMap(Shader& shader, glm::mat4 globalTransform)
{
	glm::mat4 m_GlobalTransform = globalTransform * m_LocalTransform;
	for (auto& pair : m_Components) {
		pair.second -> RenderDepthMap(shader, m_GlobalTransform);
	}
}


void Entity::Render(Shader& shader, Camera& camera, glm::mat4 globalTranform)
{
	glm::mat4 m_GlobalTransform = globalTranform * m_LocalTransform;
	for (auto& pair : m_Components) {
		pair.second -> Render(shader, camera, m_GlobalTransform);
	}
}

void Entity::Update(float deltaTime)
{
	// 初始化变换矩阵为单位矩阵
	glm::mat4 transform = glm::mat4(1.0f);

	// 应用缩放
	transform = glm::scale(transform, m_Scale);

	// 应用旋转，注意旋转的顺序
	transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

	// 应用平移
	transform = glm::translate(transform, m_Position);

	// 更新本地变换矩阵
	m_LocalTransform = transform;

	// 更新组件
	for (auto& pair : m_Components) {
		pair.second->Update(deltaTime);
	}
}

std::vector<Vertex> Entity::GetVertices(glm::mat4 globalTransform)
{
	glm::mat4 m_GlobalTransform = globalTransform * m_LocalTransform;
	std::vector<Vertex> vertices;
	for (auto& pair : m_Components) {
		if (pair.second->GetType() == "MeshComponent") {
			std::vector<Vertex> meshVertices = ((MeshComponent*)pair.second)->GetVertices(m_GlobalTransform); // 临时保存结果
			vertices.insert(vertices.end(), meshVertices.begin(), meshVertices.end());
		}
	}
	return vertices;
}

std::vector<unsigned int> Entity::GetIndices()
{
	std::vector<unsigned int> indices;
	for (auto& pair : m_Components) {
        if (pair.second->GetType() == "MeshComponent") {
			std::vector<unsigned int> meshIndices = ((MeshComponent*)pair.second)->GetIndices();
            indices.insert(indices.end(), meshIndices.begin(), meshIndices.end());
		}
	}
	return indices;
}
