#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.inl>

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
{
    m_ProjectionMatrix = glm::perspective(
        glm::radians(fov),      // 视角 (Field of View)
        aspectRatio,            // 宽高比
        nearClip,              // 近平面
        farClip                // 远平面
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
    m_ViewMatrix = glm::lookAt(
        m_CameraPosition,  // 摄像机的位置
        m_TargetPosition,  // 摄像机正在看的目标
        m_UpDirection         // 摄像机的上方向
    );
}

void Camera::SetRotation(const float& yaw, const float& pitch)
{
    glm::vec3 cameraDirection = m_TargetPosition - m_CameraPosition;
    float length = glm::length(cameraDirection);
    glm::vec3 z_Axis= glm::normalize(m_UpDirection);
    glm::vec3 x_Axis = glm::normalize(cameraDirection);
    glm::vec3 y_Axis = glm::normalize(glm::cross(z_Axis, x_Axis));
    
    // 偏航角 (yaw) 和俯仰角 (pitch)


    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDirection = glm::normalize(front);

    m_TargetPosition = m_CameraPosition + length * cameraDirection;

    glm::mat4 viewMatrix = glm::lookAt(m_CameraPosition, m_TargetPosition, m_UpDirection);
}

void Camera::Update(float deltaTime)
{
    return;
}
