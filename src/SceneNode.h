#pragma once
#include "Entity.h"
#include <vector>
#include "Camera.h"
#include <string>

class SceneNode {
private:
    std::string m_Name;
    glm::mat4 m_LocalTransform;
    std::vector<SceneNode*> m_Children;
    Entity* m_Entity;
    SceneNode* m_Parent;
public:
    SceneNode(std::string name, Entity* entity, SceneNode* parent);
    glm::mat4 GetLocalTransform() const;          // ��ȡ�ֲ��任����
    glm::mat4 GetGlobalTransform() const;         // ��ȡȫ�ֱ任����
    void AddChild(SceneNode* child);              // ����ӽڵ�
    void RemoveChild(SceneNode* child);           // �Ƴ��ӽڵ�
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);
    inline std::string GetName() const { return m_Name; }
    void Update(float deltaTime);
};