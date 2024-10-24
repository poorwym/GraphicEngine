#pragma once
#include "Component.h"

class Entity {
public:
    Entity();
    ~Entity();

    unsigned int GetID() const;                   // 获取实体的唯一ID
    void AddComponent(Component* component);      // 添加组件
    void RemoveComponent(Component* component);   // 移除组件
    template<typename T>
    T* GetComponent() const;                      // 获取指定类型的组件

    void Update(float deltaTime);                 // 更新实体
};