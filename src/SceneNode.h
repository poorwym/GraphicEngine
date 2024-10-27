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
    std::map<std::string, SceneNode*> m_Children;
    glm::vec3 m_Position;
    glm::vec3 m_Rotation;
    Entity* m_Entity;
    PointLight* m_PointLight;
    SceneNode* m_Parent;
public:
    SceneNode(std::string name, Entity* entity, SceneNode* parent);
    SceneNode(std::string name, PointLight* entity, SceneNode* parent);
    glm::mat4 GetLocalTransform() const;          // ��ȡ�ֲ��任����
    glm::mat4 GetGlobalTransform() const;         // ��ȡȫ�ֱ任����
    void AddChild(SceneNode* child);              // ����ӽڵ�
    void RemoveChild(SceneNode* child);           // �Ƴ��ӽڵ�
    void BindLight(Shader& shader, glm::mat4 globalTranform);
    void RenderDepthMap(Shader& shader, glm::mat4 globalTranform, glm::vec3 lightDir);
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);
    inline std::string GetName() const { return m_Name; }
    inline glm::vec3 GetPosition() const { return m_Position; }
    inline glm::vec3 GetRotation() const { return m_Rotation; }
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void Update(float deltaTime);
    void OnImGuiTree();
};