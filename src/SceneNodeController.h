#pragma once
#pragma once
#include "SceneNode.h"
#include <string>

class SceneNodeController {
public:
    SceneNodeController(SceneNode* sceneNode);

    void Update(float deltaTime);                 // 根据输入更新实体的状态
    void TranslateSceneNode(const glm::vec3& direction);  // 移动物体
    void RotateSceneNode(const glm::vec3& rotation); // 旋转物体
    void SelectSceneNode(SceneNode* sceneNode);            // 选择一个实体
    void DeselectSceneNode();                        // 取消选择
    void OnImGuiRender();

private:
    SceneNode* m_ControlledSceneNode;
};