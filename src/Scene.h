#pragma once
#include <string>
#include "Entity.h"
#include <vector>

class Scene {
public:
    void Load(const std::string& filePath);   // ���س���
    void Save(const std::string& filePath);   // ���泡��
    void Update(float deltaTime);             // ���³���
    void AddEntity(Entity* entity);           // ���ʵ��
    void RemoveEntity(Entity* entity);        // ɾ��ʵ��
    const std::vector<Entity*>& GetEntities() const; // ��ȡ�����е�ʵ���б�
};