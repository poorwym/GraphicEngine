#pragma once
#include "Component.h"

class Entity {
public:
    Entity();
    ~Entity();

    unsigned int GetID() const;                   // ��ȡʵ���ΨһID
    void AddComponent(Component* component);      // ������
    void RemoveComponent(Component* component);   // �Ƴ����
    template<typename T>
    T* GetComponent() const;                      // ��ȡָ�����͵����

    void Update(float deltaTime);                 // ����ʵ��
};