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
    unsigned int m_RendererID;
public:
    void load(const std::string& filePath);   // ���س���
    void save(const std::string& filePath);   // ���泡��
    void Update(float deltaTime);             // ���³���
    void AddNode(SceneNode* node);           // ���ʵ��
    void Render(Shader& shader,Camera& camera);
    void RemoveNode(SceneNode* node);        // ɾ��ʵ��
};