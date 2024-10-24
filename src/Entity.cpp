#include "Entity.h"

Entity::Entity()
	: m_ID(0)
{
	m_LocalTransform = glm::mat4(1.0f);

}
