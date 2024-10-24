#pragma once
#include <string>
#include "Entity.h"
#include <vector>
#include "SceneNode.h"

class Scene {
private:
    std::vector<SceneNode *> sceneNodes;
    unsigned int m_RendererID;
public:
    void load(const std::string& filePath);   // ���س���
    void save(const std::string& filePath);   // ���泡��
    void update(float deltaTime);             // ���³���
    void addNode(SceneNode* node);           // ���ʵ��
    void removeNode(SceneNode* node);        // ɾ��ʵ��
};