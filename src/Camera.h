#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <string>
#include "Shader.h"

class Camera {
private:
    glm::vec3 m_CameraPosition;
    glm::vec3 m_TargetPosition;
    glm::vec3 m_UpDirection;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
    float m_AspectRatio;
    float m_NearClip;
    float m_FarClip;
    float m_Fov;
public:
    Camera(float fov, float aspectRatio, float nearClip, float farClip);
    inline glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }      // ��ȡ��ͼ����
    inline glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }// ��ȡͶӰ����
    void SetPosition(const glm::vec3& position);
    void SetRotation(const float& yaw, const float& pitch);
    void SetTarget(const glm::vec3& target);
    void ProcessKeyboard(char pressedKey, float deltaTime, float velocity);// ���̿���
    void ProcessMouseMovement(float xOffset, float yOffset);
    void ProcessMouseScroll(float zoomAngle);
    void Bind(Shader& shader);
    void Update(float deltaTime);         // ����������������
    inline glm::vec3 GetPosition() const { return m_CameraPosition; }
};