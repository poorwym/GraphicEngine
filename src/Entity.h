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

    void AddComponent(Component* component);      // 添加组件
    void RemoveComponent(Component* component);   // 移除组件
    inline glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
    void Rotate(glm::vec3 rotation);
    void Translate(glm::vec3 translation);
    void Scale(glm::vec3 scale);
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);
    void Update(float deltaTime);                 // 更新实体
};