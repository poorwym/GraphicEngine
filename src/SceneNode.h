#pragma once
#include "Entity.h"
#include <vector>

class SceneNode {
public:
    Entity* entity;
    SceneNode* parent;
    std::vector<SceneNode*> children;

    glm::mat4 GetLocalTransform() const;          // 获取局部变换矩阵
    glm::mat4 GetGlobalTransform() const;         // 获取全局变换矩阵
    void AddChild(SceneNode* child);              // 添加子节点
    void RemoveChild(SceneNode* child);           // 移除子节点
};