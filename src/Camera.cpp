#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
    :m_CameraPosition(glm::vec3(0.0f, 0.0f, 3.0f)),
    m_TargetPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_UpDirection(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_Fov(fov),m_AspectRatio(aspectRatio),
    m_NearClip(nearClip), m_FarClip(farClip)
{
    m_ProjectionMatrix = glm::perspective(
        glm::radians(m_Fov),      // 视角 (Field of View)
        m_AspectRatio,            // 宽高比
        m_NearClip,              // 近平面
        m_FarClip                // 远平面
    );
    m_ViewMatrix = glm::lookAt(
        m_CameraPosition,  // 摄像机的位置
        m_TargetPosition,  // 摄像机正在看的目标
        m_UpDirection         // 摄像机的上方向
    );
}

void Camera::SetPosition(const glm::vec3& position)
{
    m_CameraPosition = position;
    this->Update(0.0f);
}

void Camera::SetRotation(const float& yaw, const float& pitch)
{
    glm::vec3 cameraDirection = m_TargetPosition - m_CameraPosition;
    float length = glm::length(cameraDirection);
    glm::vec3 forward = glm::normalize(cameraDirection);
    glm::vec3 right = glm::normalize(glm::cross(forward, m_UpDirection));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));
    
    // 偏航角 (yaw) 和俯仰角 (pitch)

    forward = glm::normalize(forward * cos(glm::radians(yaw)) + right * sin(glm::radians(yaw)));
    right = glm::normalize(glm::cross(forward, m_UpDirection));

    forward = glm::normalize(forward * cos(glm::radians(pitch)) + up * sin(glm::radians(pitch)));

    m_TargetPosition = m_CameraPosition + length * forward;
}

void Camera::SetTarget(const glm::vec3& target)
{
    m_TargetPosition = target;
    this->Update(0.0f);
}

void Camera::ProcessKeyboard(char pressedKey, float deltaTime, float velocity)
{
    glm::vec3 orientation = m_TargetPosition - m_CameraPosition;
    orientation.y = 0.0f;
    glm::vec3 z_Axis = glm::normalize(orientation);
    glm::vec3 x_Axis = glm::normalize(glm::cross(z_Axis, m_UpDirection));
    if (pressedKey == 'W') {
        m_CameraPosition += z_Axis * velocity;
        m_TargetPosition += z_Axis * velocity;
    }
    if (pressedKey == 'S') {
        m_CameraPosition -= z_Axis * velocity;
        m_TargetPosition -= z_Axis * velocity;
    }
    if (pressedKey == 'D') {
        m_CameraPosition += x_Axis * velocity;
        m_TargetPosition += x_Axis * velocity;
    }
    if (pressedKey == 'A') {
        m_CameraPosition -= x_Axis * velocity;
        m_TargetPosition -= x_Axis * velocity;
    }
    this->Update(deltaTime);
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
    SetRotation(xOffset, yOffset);
    this->Update(0.0);
}

void Camera::ProcessMouseScroll(float zoomAngle)
{
    m_Fov -= zoomAngle;
    if (m_Fov < 1.0f) m_Fov = 1.0f;
    if (m_Fov > 170.0f) m_Fov = 170.0f;
    this->Update(0.0f);
}

void Camera::Update(float deltaTime)
{
    m_ProjectionMatrix = glm::perspective(
        glm::radians(m_Fov),      // 视角 (Field of View)
        m_AspectRatio,            // 宽高比
        m_NearClip,              // 近平面
        m_FarClip                // 远平面
    );
    m_ViewMatrix = glm::lookAt(
        m_CameraPosition,  // 摄像机的位置
        m_TargetPosition,  // 摄像机正在看的目标
        m_UpDirection         // 摄像机的上方向
    );
    return;
}
