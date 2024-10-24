#pragma once
#include "Camera.h"

class CameraController {
public:
    CameraController(Camera* camera);

    void Update(float deltaTime);               // ����������������
    void ProcessKeyboardInput(float deltaTime); // �������������������
    void ProcessMouseInput(float xOffset, float yOffset); // �������������������
    void ProcessMouseScroll(float yOffset);     // ��������������

private:
    Camera* camera;
    float movementSpeed;
    float mouseSensitivity;
    float zoomLevel;
};