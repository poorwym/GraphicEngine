#include "CameraController.h"

CameraController::CameraController(Camera* camera, GLFWwindow* window)
    : camera(camera), window(window), movementSpeed(2.5f), mouseSensitivity(0.1f), zoomLevel(45.0f),
    lastX(0.0f), lastY(0.0f), firstMouse(true),locked(true)
{
    // ���ع�겢���񵽴�����
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
    // ��������ڻص������д���
}

void CameraController::ProcessKeyboardInput(float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    // ��鰴��״̬
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
    float yOffset = lastY - ypos; // ע�������� lastY - ypos

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
    // �������Ƿ񱻰���
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (!leftButtonPressed)
        {
            leftButtonPressed = true;
        }
        else
        {
            // ����������£�ִ�г���������������ת�����
            // ������Ը�����Ҫʵ����ת�߼�
        }
    }
    else
    {
        if (leftButtonPressed)
        {
            leftButtonPressed = false;
            // ��������ͷ��¼�������ֹͣ��ת�����
        }
    }

    // ����Ҽ��Ƿ񱻰���
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (!rightButtonPressed)
        {
            rightButtonPressed = true;
            // �����Ҽ������¼������翪ʼƽ�������
        }
        else
        {
            locked = false;
            // �Ҽ��������£�ִ�г�������������ƽ�������
        }
    }
    else
    {
        if (rightButtonPressed)
        {
            rightButtonPressed = false;
            locked = true;
            // �����Ҽ��ͷ��¼�������ֹͣƽ�������
        }
    }
}
