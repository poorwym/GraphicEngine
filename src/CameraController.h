#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

class CameraController {
public:
    CameraController(Camera* camera, GLFWwindow* window);
    ~CameraController();

    void Update(float deltaTime);               // 根据输入更新摄像机
    void ProcessKeyboardInput(float deltaTime); // 处理键盘输入控制摄像机
    void ProcessMouseInput(float xpos, float ypos); // 处理鼠标输入控制摄像机
    void ProcessMouseScroll(float yOffset);     // 处理鼠标滚轮缩放
    void ProcessMouseButtons(float deltaTime);            // 处理鼠标按钮输入
    void OnImGuiRender();

private:
    Camera* camera;
    GLFWwindow* window;
    bool locked;
    float movementSpeed;
    float mouseSensitivity;
    float zoomLevel;

    float lastX;
    float lastY;
    bool firstMouse;

    // 记录鼠标按钮的按下状态
    bool leftButtonPressed;
    bool rightButtonPressed;
};
