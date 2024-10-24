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

    inline unsigned int GetID() const { return m_ID; };                   // ��ȡʵ���ΨһID
    void AddComponent(Component* component);      // ������
    void RemoveComponent(Component* component);   // �Ƴ����
    template<typename T>
    T* GetComponent() const;                      // ��ȡָ�����͵����

    void Update(float deltaTime);                 // ����ʵ��
};

extern std::map<unsigned int, Entity*> EntityList;

template<typename T>
inline T* Entity::GetComponent() const
{
    return nullptr;
}
