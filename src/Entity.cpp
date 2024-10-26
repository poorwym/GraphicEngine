#include "Entity.h"

Entity::Entity()
{
	m_LocalTransform = glm::mat4(1.0f);
}

Entity::~Entity()
{
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


void Entity::Render(Shader& shader, Camera& camera, glm::mat4 globalTranform)
{
	glm::mat4 m_GlobalTransform = globalTranform * m_LocalTransform;
	for (auto& pair : m_Components) {
		pair.second -> Render(shader, camera, m_GlobalTransform);
	}
}

void Entity::Update(float deltaTime)
{

}
