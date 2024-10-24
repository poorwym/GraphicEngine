#pragma once
#include "Entity.h"
#include <vector>

class SceneNode {
public:
    Entity* entity;
    SceneNode* parent;
    std::vector<SceneNode*> children;

    glm::mat4 GetLocalTransform() const;          // ��ȡ�ֲ��任����
    glm::mat4 GetGlobalTransform() const;         // ��ȡȫ�ֱ任����
    void AddChild(SceneNode* child);              // ����ӽڵ�
    void RemoveChild(SceneNode* child);           // �Ƴ��ӽڵ�
};