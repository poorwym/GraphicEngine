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
    void load(const std::string& filePath);   // 加载场景
    void save(const std::string& filePath);   // 保存场景
    void Update(float deltaTime);             // 更新场景
    void AddNode(SceneNode* node);           // 添加实体
    void Render(Shader& shader,Camera& camera);
    void RemoveNode(SceneNode* node);        // 删除实体
};