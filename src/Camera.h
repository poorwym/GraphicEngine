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
    float m_Rate;
    float m_FocusDepth;   // 焦点深度
    float m_FocusRange;   // 焦点范围（过渡区域）
    float m_MaxBlur;      // 最大模糊半径

public:
    Camera(float fov, float aspectRatio, float nearClip, float farClip);
    inline glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }      // 获取视图矩阵
    inline glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }// 获取投影矩阵
    void SetPosition(const glm::vec3& position);
    void SetRotation(const float& yaw, const float& pitch);
    void SetTarget(const glm::vec3& target);
    inline float GetFocusDepth() const {return m_FocusDepth;}
    inline float GetFocusRange() const {return m_FocusRange;}
    inline float GetMaxBlur() const {   return m_MaxBlur;}
    void SetFocus(float focusDepth, float focusRange, float maxBlur);
    void ProcessKeyboard(std::string pressedKey, float deltaTime, float velocity);// 键盘控制
    void ProcessMouseMovement(float xOffset, float yOffset);
    void ProcessMouseScroll(float zoomAngle);
    void Bind(Shader& shader);
    void Update(float deltaTime);         // 根据输入更新摄像机
    inline glm::vec3 GetPosition() const { return m_CameraPosition; }
    inline glm::vec3 GetTarget() const { return m_TargetPosition; }
};