#pragma once
#include <string>
#include "Entity.h"
#include <vector>

class Scene {
public:
    void Load(const std::string& filePath);   // 加载场景
    void Save(const std::string& filePath);   // 保存场景
    void Update(float deltaTime);             // 更新场景
    void AddEntity(Entity* entity);           // 添加实体
    void RemoveEntity(Entity* entity);        // 删除实体
    const std::vector<Entity*>& GetEntities() const; // 获取场景中的实体列表
};