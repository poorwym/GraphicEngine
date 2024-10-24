#pragma once
#include "Entity.h"
#include <vector>

class SceneNode {
private:
    glm::mat4 m_LocalTransform;
    std::vector<SceneNode*> children;
public:
    Entity* entity;
    SceneNode* parent;

    glm::mat4 GetLocalTransform() const;          // 获取局部变换矩阵
    glm::mat4 GetGlobalTransform() const;         // 获取全局变换矩阵
    void AddChild(SceneNode* child);              // 添加子节点
    void RemoveChild(SceneNode* child);           // 移除子节点
    void Update(float deltaTime);
    inline unsigned int GetID() const { return entity->GetID(); }
};