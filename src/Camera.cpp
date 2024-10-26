#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
    :m_CameraPosition(glm::vec3(0.0f, 0.0f, 3.0f)),
    m_TargetPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_UpDirection(glm::vec3(0.0f, 1.0f, 0.0f))
{
    m_ProjectionMatrix = glm::perspective(
        glm::radians(fov),      // �ӽ� (Field of View)
        aspectRatio,            // ��߱�
        nearClip,              // ��ƽ��
        farClip                // Զƽ��
    );
    m_ViewMatrix = glm::lookAt(
        m_CameraPosition,  // �������λ��
        m_TargetPosition,  // ��������ڿ���Ŀ��
        m_UpDirection         // ��������Ϸ���
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
    glm::vec3 z_Axis= glm::normalize(m_UpDirection);
    glm::vec3 x_Axis = glm::normalize(cameraDirection);
    glm::vec3 y_Axis = glm::normalize(glm::cross(z_Axis, x_Axis));
    
    // ƫ���� (yaw) �͸����� (pitch)

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDirection = glm::normalize(front);

    m_TargetPosition = m_CameraPosition + length * cameraDirection;
    this->Update(0.0f);
}

void Camera::SetTarget(const glm::vec3& target)
{
    m_TargetPosition = target;
    this->Update(0.0f);
}

void Camera::Update(float deltaTime)
{
    m_ViewMatrix = glm::lookAt(
        m_CameraPosition,  // �������λ��
        m_TargetPosition,  // ��������ڿ���Ŀ��
        m_UpDirection         // ��������Ϸ���
    );
    return;
}
