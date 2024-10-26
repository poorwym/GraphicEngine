#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>

class CameraController {
public:
    CameraController(Camera* camera, GLFWwindow* window);

    void Update(float deltaTime);               // ����������������
    void ProcessKeyboardInput(float deltaTime); // �������������������
    void ProcessMouseInput(float xpos, float ypos); // �������������������
    void ProcessMouseScroll(float yOffset);     // ��������������
    void ProcessMouseButtons(float deltaTime);            // ������갴ť����

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

    // ��¼��갴ť�İ���״̬
    bool leftButtonPressed;
    bool rightButtonPressed;
};
