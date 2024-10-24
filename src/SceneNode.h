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

    glm::mat4 GetLocalTransform() const;          // ��ȡ�ֲ��任����
    glm::mat4 GetGlobalTransform() const;         // ��ȡȫ�ֱ任����
    void AddChild(SceneNode* child);              // ����ӽڵ�
    void RemoveChild(SceneNode* child);           // �Ƴ��ӽڵ�
    void Update(float deltaTime);
    inline unsigned int GetID() const { return entity->GetID(); }
};