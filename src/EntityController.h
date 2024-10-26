#pragma once
#include "Entity.h"

class EntityController {
public:
    EntityController(Entity* entity);

    void Update(float deltaTime);                 // 根据输入更新实体的状态
    void TranslateEntity(const glm::vec3& direction);  // 移动物体
    void RotateEntity(const glm::vec3& rotation); // 旋转物体
    void ScaleEntity(const glm::vec3& scale);     // 缩放物体
    void SelectEntity(Entity* entity);            // 选择一个实体
    void DeselectEntity();                        // MoveEntity取消选择
    void OnImGuiRender();

private:
    Entity* m_ControlledEntity;
};