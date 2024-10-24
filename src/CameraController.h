#pragma once
#include "Camera.h"

class CameraController {
public:
    CameraController(Camera* camera);

    void Update(float deltaTime);               // 根据输入更新摄像机
    void ProcessKeyboardInput(float deltaTime); // 处理键盘输入控制摄像机
    void ProcessMouseInput(float xOffset, float yOffset); // 处理鼠标输入控制摄像机
    void ProcessMouseScroll(float yOffset);     // 处理鼠标滚轮缩放

private:
    Camera* camera;
    float movementSpeed;
    float mouseSensitivity;
    float zoomLevel;
};