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
    void load(const std::string& filePath);   // 加载场景
    void save(const std::string& filePath);   // 保存场景
    void update(float deltaTime);             // 更新场景
    void addNode(SceneNode* node);           // 添加实体
    void removeNode(SceneNode* node);        // 删除实体
};