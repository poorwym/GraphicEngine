#pragma once
#include "Entity.h"
#include <vector>
#include "Camera.h"
#include <string>
#include <map>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Light.h"


class SceneNode {
private:
    std::string m_Name;
    glm::mat4 m_LocalTransform;
    glm::mat4 m_PreviousTransform;
    std::map<std::string, SceneNode*> m_Children;
    glm::vec3 m_Position;
    glm::vec3 m_Rotation;
    Entity* m_Entity;
    PointLight* m_PointLight;
    DirectionalLight* m_DirectionalLight;
    SpotLight* m_SpotLight;
    SceneNode* m_Parent;

public:
    bool deleted = false;

    SceneNode(std::string name, void* ptr, SceneNode* parent);
    SceneNode(std::string name, Entity* entity, SceneNode* parent);
    SceneNode(std::string name, PointLight* entity, SceneNode* parent);
    SceneNode(std::string name, DirectionalLight* directionalLight, SceneNode* parent);
    SceneNode(std::string name, SpotLight* spotLight, SceneNode* parent);

    ~SceneNode();
    glm::mat4 GetLocalTransform() const;          // 获取局部变换矩阵
    glm::mat4 GetGlobalTransform() const;         // 获取全局变换矩阵
    void AddChild(SceneNode* child);              // 添加子节点
    void RemoveChild(SceneNode* child);           // 移除子节点
    void BindLight(Shader& shader, glm::mat4 globalTranform);
    void RenderDepthMap(Shader& shader, glm::mat4 globalTranform);
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);
    inline std::string GetName() const { return m_Name; }
    inline glm::vec3 GetPosition() const { return m_Position; }
    inline glm::vec3 GetRotation() const { return m_Rotation; }
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void Update(float deltaTime);
    void OnImGuiTree();
    void DeleteSelf();

    std::vector<std::vector<Vertex>*> GetVertices(glm::mat4 globalTranform);
    std::vector<std::vector<unsigned int>*> GetIndices();
};