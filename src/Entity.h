#pragma once
#include "Component.h"
#include <map>

class Entity {
private:
    unsigned int m_ID;
    std::map<unsigned int, Component*> m_Components;
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

extern std::map<unsigned int, Entity*> EntityList;

template<typename T>
inline T* Entity::GetComponent() const
{
    return nullptr;
}
