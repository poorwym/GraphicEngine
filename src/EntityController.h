#pragma once
#include "Entity.h"

class EntityController {
public:
    EntityController(Entity* entity);

    void Update(float deltaTime);                 // �����������ʵ���״̬
    void MoveEntity(const glm::vec3& direction);  // �ƶ�����
    void RotateEntity(const glm::vec3& axis, float angle); // ��ת����
    void ScaleEntity(const glm::vec3& scale);     // ��������

    void SelectEntity(Entity* entity);            // ѡ��һ��ʵ��
    void DeselectEntity();                        // ȡ��ѡ��

private:
    Entity* controlledEntity;
};