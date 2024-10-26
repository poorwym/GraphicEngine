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
    glm::mat4 GetLocalTransform() const;          // 获取局部变换矩阵
    glm::mat4 GetGlobalTransform() const;         // 获取全局变换矩阵
    void AddChild(SceneNode* child);              // 添加子节点
    void RemoveChild(SceneNode* child);           // 移除子节点
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);
    inline std::string GetName() const { return m_Name; }
    void Update(float deltaTime);
};