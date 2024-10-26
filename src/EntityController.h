#pragma once
#include "Entity.h"

class EntityController {
public:
    EntityController(Entity* entity);

    void Update(float deltaTime);                 // �����������ʵ���״̬
    void TranslateEntity(const glm::vec3& direction);  // �ƶ�����
    void RotateEntity(const glm::vec3& rotation); // ��ת����
    void ScaleEntity(const glm::vec3& scale);     // ��������
    void SelectEntity(Entity* entity);            // ѡ��һ��ʵ��
    void DeselectEntity();                        // MoveEntityȡ��ѡ��
    void OnImGuiRender();

private:
    Entity* m_ControlledEntity;
};