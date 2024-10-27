#pragma once

#include <map>
#include <glm/glm.hpp>
#include "Component.h"
#include <string>

class Entity {
    friend class EntityController;
private:
    std::map<unsigned int, Component*> m_Components;
    glm::mat4 m_LocalTransform;
    std::string m_Name;
    glm::vec3 m_Scale;
    glm::vec3 m_Rotation;
    glm::vec3 m_Position;
public:
    Entity(std::string name);
    ~Entity();

    inline std::string GetName() { return m_Name; }
    void AddComponent(Component* component);      // 添加组件
    void RemoveComponent(Component* component);   // 移除组件
    inline glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
    inline glm::vec3 GetPosition() const { return m_Position; }
    inline glm::vec3 GetRotation() const { return m_Rotation; }
    inline glm::vec3 GetScale() const { return m_Scale; }
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetScale(glm::vec3 scale);
    void Rotate(glm::vec3 rotation);
    void Translate(glm::vec3 translation);
    void Scale(glm::vec3 scale);
    void RenderDepthMap(Shader& shader, glm::mat4 globalTransform, glm::vec3 lightDir);
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);
    void Update(float deltaTime);                 // 更新实体
};