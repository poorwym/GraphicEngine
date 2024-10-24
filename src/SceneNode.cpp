#include "SceneNode.h"

void SceneNode::AddChild(SceneNode* child)
{
    children.push_back(child);
}

void SceneNode::RemoveChild(SceneNode* child)
{
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

glm::mat4 SceneNode::GetGlobalTransform() const
{
    return parent ? parent->GetGlobalTransform() * GetLocalTransform() : GetLocalTransform();
}

glm::mat4 SceneNode::GetLocalTransform() const
{
    return entity ? entity->GetTransform() : glm::mat4(1.0f);
}