#include "CameraController.h"

CameraController::CameraController(Camera* camera, GLFWwindow* window)
    : camera(camera), window(window), movementSpeed(2.5f), mouseSensitivity(0.1f), zoomLevel(45.0f),
    lastX(0.0f), lastY(0.0f), firstMouse(true),locked(true)
{
    // 隐藏光标并捕获到窗口中
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

CameraController::~CameraController()
{
    delete camera;
    delete window;
}

void CameraController::Update(float deltaTime)
{
    ProcessKeyboardInput(deltaTime);
    ProcessMouseButtons(deltaTime);
    // 鼠标输入在回调函数中处理
}

void CameraController::ProcessKeyboardInput(float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    // 检查按键状态
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard("W", deltaTime, velocity);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard("S", deltaTime, velocity);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard("A", deltaTime, velocity);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard("D", deltaTime, velocity);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera->ProcessKeyboard("UP", deltaTime, velocity);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera->ProcessKeyboard("DOWN", deltaTime, velocity);
}

void CameraController::ProcessMouseInput(float xpos, float ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // 注意这里是 lastY - ypos

    lastX = xpos;
    lastY = ypos;

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    if(!locked) camera->ProcessMouseMovement(xOffset, yOffset);
}

void CameraController::ProcessMouseScroll(float yOffset)
{
    float zoomAngle = zoomLevel * yOffset * 0.1f;
    camera->ProcessMouseScroll(zoomAngle);
}

void CameraController::ProcessMouseButtons(float deltaTime)
{
    // 检查左键是否被按下
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (!leftButtonPressed)
        {
            leftButtonPressed = true;
        }
        else
        {
            // 左键持续按下，执行持续操作，例如旋转摄像机
            // 这里可以根据需要实现旋转逻辑
        }
    }
    else
    {
        if (leftButtonPressed)
        {
            leftButtonPressed = false;
            // 处理左键释放事件，例如停止旋转摄像机
        }
    }

    // 检查右键是否被按下
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (!rightButtonPressed)
        {
            rightButtonPressed = true;
            // 处理右键按下事件，例如开始平移摄像机
        }
        else
        {
            locked = false;
            // 右键持续按下，执行持续操作，例如平移摄像机
        }
    }
    else
    {
        if (rightButtonPressed)
        {
            rightButtonPressed = false;
            locked = true;
            // 处理右键释放事件，例如停止平移摄像机
        }
    }
}
