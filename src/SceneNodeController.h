#pragma once
#pragma once
#include "SceneNode.h"
#include <string>

class SceneNodeController {
public:
    SceneNodeController(SceneNode* sceneNode);

    void Update(float deltaTime);                 // �����������ʵ���״̬
    void TranslateSceneNode(const glm::vec3& direction);  // �ƶ�����
    void RotateSceneNode(const glm::vec3& rotation); // ��ת����
    void SelectSceneNode(SceneNode* sceneNode);            // ѡ��һ��ʵ��
    void DeselectSceneNode();                        // ȡ��ѡ��
    void OnImGuiRender();

private:
    SceneNode* m_ControlledSceneNode;
};