#pragma once
#include <string>
#include "Entity.h"
#include <vector>
#include "SceneNode.h"
#include "Camera.h"
#include <map>

class Scene {
private:
    std::map<std::string, SceneNode *> m_SceneNodes;
    DirectionalLight* m_DirLight;
    unsigned int m_RendererID;
public:
    Scene();
    void load(const std::string& filePath);   // ���س���
    void save(const std::string& filePath);   // ���泡��
    void Update(float deltaTime);             // ���³���
    void AddNode(SceneNode* node);           // ���ʵ��
    void SetDirectionalLight(DirectionalLight* dirLight);
    void BindLight(Shader& shader, glm::mat4 globalTransform);
    void RenderDepthMap(Shader& shader);
    void Render(Shader& shader,Camera& camera);
    void RemoveNode(SceneNode* node);        // ɾ��ʵ��
    void OnImGuiTree();
};