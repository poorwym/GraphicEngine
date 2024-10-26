#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

class Light {
protected:
    glm::vec3 m_Color;
    float m_Intensity;
public:
    Light(glm::vec3 color, float intensity) :m_Color(color), m_Intensity(intensity) {};
    void SetLightColor(glm::vec3 color);
    void SetLightIntensity(float intensity);
    inline glm::vec3 GetLightColor() const { return m_Color; };
    inline float GetLightIntensity() const { return m_Intensity; };
    // 不同光源类型可以继承此类
    virtual void Bind(Shader& shader) {};
    virtual std::string GetType() const { return "Light"; };
    virtual void Update(float deltaTime);
};

class DirectionalLight : public Light {
private:
    glm::vec3 m_LightDir;       // 方向光方向
    glm::vec3 m_LightAmbient;   // 环境光强度
    glm::vec3 m_LightDiffuse;   // 漫反射光强度
    glm::vec3 m_LightSpecular;  // 镜面光强度
public:
    DirectionalLight(glm::vec3 color, float intensity, glm::vec3 lightDir, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular);
    DirectionalLight(glm::vec3 color, float intensity, glm::vec3 lightDir);
    void SetLightDir(glm::vec3 lightDir);
    void SetLightAmbient(glm::vec3 ambient);
    void SetLightDiffuse(glm::vec3 diffuse);
    void SetLightSpecular(glm::vec3 specular);
    inline glm::vec3 GetLightSpecular() const { return m_LightSpecular; };
    inline glm::vec3 GetLightDir() const { return m_LightDir; };
    inline glm::vec3 GetLightAmbient() const { return m_LightAmbient; };
    inline glm::vec3 GetLightDiffuse() const { return m_LightDiffuse; };
    void Bind(Shader& shader) override;
    virtual std::string GetType() const override { return "DirectionalLight"; };
    void Update(float deltaTime) override;
};

class PointLight : public Light {
private:
    glm::vec3 m_LightPos;
public:
    float range;
    void Bind(Shader& shader) override;  // 将光照数据传递给着色器
};