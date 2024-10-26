#include "SceneNode.h"
#include "Entity.h"

void SceneNode::AddChild(SceneNode* child)
{
    m_Children.push_back(child);
}

void SceneNode::RemoveChild(SceneNode* child)
{
    m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), child), m_Children.end());
}

void SceneNode::Render(Shader& shader, Camera& camera, glm::mat4 globalTranform)
{
    glm::mat4 m_GlobalTransform = globalTranform * m_LocalTransform;
    if (m_Entity){
        m_Entity->Render(shader, camera, m_GlobalTransform);
    }
    for (auto child : m_Children) {
        child->Render(shader, camera, m_GlobalTransform);
    }
}

void SceneNode::Update(float deltaTime)
{
    m_Entity->Update(deltaTime);
    for (auto child : m_Children)
    {
        child->Update(deltaTime);
    }
}

glm::mat4 SceneNode::GetGlobalTransform() const
{
    return m_Parent ? m_Parent->GetGlobalTransform() * GetLocalTransform() : GetLocalTransform();
}

SceneNode::SceneNode(std::string name, Entity* entity, SceneNode* parent)
    :m_LocalTransform(glm::mat4(1.0f)),m_Name(name)
{
    m_Entity = entity;
    m_Parent = parent;
}

glm::mat4 SceneNode::GetLocalTransform() const
{
    return m_Entity ? m_Entity->GetLocalTransform() : glm::mat4(1.0f);
}