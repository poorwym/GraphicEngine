#include "SceneNode.h"

void SceneNode::AddChild(SceneNode* child)
{
    m_Children.push_back(child);
}

void SceneNode::RemoveChild(SceneNode* child)
{
    m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), child), m_Children.end());
}

void SceneNode::Update(float deltaTime)
{
    for (auto child : m_Children)
    {
        child->Update(deltaTime);
    }
}

glm::mat4 SceneNode::GetGlobalTransform() const
{
    return m_Parent ? m_Parent->GetGlobalTransform() * GetLocalTransform() : GetLocalTransform();
}

SceneNode::SceneNode(Entity* entity, SceneNode* parent)
{
    m_Entity = entity;
    m_Parent = parent;
}

glm::mat4 SceneNode::GetLocalTransform() const
{
    return m_Entity ? m_Entity->GetLocalTransform() : glm::mat4(1.0f);
}