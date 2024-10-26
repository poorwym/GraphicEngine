#pragma once

#include <map>
#include <glm/glm.hpp>
#include "Component.h"

class Entity {
private:
    std::map<unsigned int, Component*> m_Components;
    glm::mat4 m_LocalTransform;
public:
    Entity();
    ~Entity();

    void AddComponent(Component* component);      // ������
    void RemoveComponent(Component* component);   // �Ƴ����
    inline glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
    void Rotate(glm::vec3 rotation);
    void Translate(glm::vec3 translation);
    void Scale(glm::vec3 scale);
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);
    void Update(float deltaTime);                 // ����ʵ��
};