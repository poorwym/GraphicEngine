#include "EntityController.h"

EntityController::EntityController(Entity* entity)
	: m_ControlledEntity(entity)
{

}

void EntityController::Update(float deltaTime)
{
	m_ControlledEntity->Update(deltaTime);
}

void EntityController::TranslateEntity(const glm::vec3& direction)
{
	m_ControlledEntity->Translate(direction);
}

void EntityController::RotateEntity(const glm::vec3& rotation)
{
	m_ControlledEntity->Rotate(rotation);
}

void EntityController::ScaleEntity(const glm::vec3& scale)
{
	m_ControlledEntity->Scale(scale);
}

void EntityController::SelectEntity(Entity* entity)
{
	m_ControlledEntity = entity;
}

void EntityController::DeselectEntity()
{
	m_ControlledEntity = nullptr;
}
