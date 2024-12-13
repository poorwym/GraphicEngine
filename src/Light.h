#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "FrameBuffer.h"

class Light {
protected:
    glm::vec3 m_Color;
    float m_Intensity;
    glm::vec3 m_LightAmbient;   // ������ǿ��
    glm::vec3 m_LightDiffuse;   // �������ǿ��
    glm::vec3 m_LightSpecular;  // �����ǿ��
    glm::vec3 m_AmbientColor;
    glm::vec3 m_DiffuseColor;
    glm::vec3 m_SpecularColor;
    std::string m_Name;
public:
    Light(const std::string& name,glm::vec3 color, float intensity);
    void SetLightColor(glm::vec3 color);
    void SetLightIntensity(float intensity);
    inline std::string GetName() const { return m_Name; };
    inline glm::vec3 GetLightColor() const { return m_Color; };
    inline float GetLightIntensity() const { return m_Intensity; };
    void SetLightAmbient(glm::vec3 ambient);
    void SetLightDiffuse(glm::vec3 diffuse);
    void SetLightSpecular(glm::vec3 specular);
    inline glm::vec3 GetLightSpecular() const { return m_LightSpecular; };
    inline glm::vec3 GetLightAmbient() const { return m_LightAmbient; };
    inline glm::vec3 GetLightDiffuse() const { return m_LightDiffuse; };
    // ��ͬ��Դ���Ϳ��Լ̳д���
    virtual void Bind(Shader& shader, glm::mat4 globalTransform) {};
    virtual std::string GetType() const { return "Light"; };
    virtual void Update(float deltaTime);
    virtual glm::mat4 ComputeLightSpaceMatrix(glm::vec3 sceneCenter) { return glm::mat4(0.0f); };
};

class DirectionalLight : public Light {
private:
    glm::vec3 m_LightDir;       // ����ⷽ��
public:
    DepthMapFBO* m_ShadowMapFBO;
    DirectionalLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightDir, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular);
    DirectionalLight(const std::string& name,glm::vec3 color, float intensity, glm::vec3 lightDir);
    void SetLightDir(glm::vec3 lightDir);
    inline glm::vec3 GetLightDir() const { return m_LightDir; };
    void Bind(Shader& shader, glm::mat4 globalTransform) override;
    std::string GetType() const override { return "DirectionalLight"; };
    void Update(float deltaTime) override;
    glm::mat4 ComputeLightSpaceMatrix(glm::vec3 sceneCenter) override;
};

class PointLight : public Light {
private:
    glm::vec3 m_LightPos;       // ���Դλ��
    // ˥������
    float m_Constant;   //����˥��
    float m_Linear;     //����˥��
    float m_Quadratic;  //���η�˥��
public:
    CubeMapFBO* m_CubeShadowMapFBO;
    PointLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightPos);
    PointLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightPos, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular, float constant, float linear, float quadratic);
    std::string GetType() const override { return "PointLight"; };
    inline glm::vec3 GetLightPos() const { return m_LightPos; };
    inline float GetConstant() const { return m_Constant; }
    inline float GetLinear() const { return m_Linear; }
    inline float GetQuadratic() const { return m_Quadratic; }
    void SetLightPos(glm::vec3 lightPos);
    void SetConstant(float constant);
    void SetLinear(float linear);
    void SetQuadratic(float quadratic);
    void Bind(Shader& shader, glm::mat4 globalTransform) override;  // ���������ݴ��ݸ���ɫ��
    void Update(float deltaTime) override;
    std::vector<glm::mat4> ComputePointLightShadowMatrices(float nearPlane, float farPlane);
};

class SpotLight : public Light {
private:
    glm::vec3 m_LightDir;          // �۹�Ʒ���
    glm::vec3 m_LightPos;           // �۹��λ��
    float m_CutOff;                // ��׶�ǣ�cosine of the angle��
    float m_OuterCutOff;           // ��׶�ǣ�cosine of the angle��
    // ˥������
    float m_Constant;   // ����˥��
    float m_Linear;     // ����˥��
    float m_Quadratic;  // ���η�˥��
public:
    DepthMapFBO* m_ShadowMapFBO;  // �����Ҫ�۹����Ӱӳ��
    SpotLight(const std::string& name, glm::vec3 color, float intensity,
        glm::vec3 lightPos, glm::vec3 lightDir,
        float cutOff, float outerCutOff);
    SpotLight(const std::string& name, glm::vec3 color, float intensity,
        glm::vec3 lightPos, glm::vec3 lightDir,
        glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular,
        float constant, float linear, float quadratic,
        float cutOff, float outerCutOff);

    std::string GetType() const override { return "SpotLight"; };

    inline glm::vec3 GetLightDir() const { return m_LightDir; };
    inline float GetCutOff() const { return m_CutOff; };
    inline float GetOuterCutOff() const { return m_OuterCutOff; };
    inline float GetConstant() const { return m_Constant; };
    inline float GetLinear() const { return m_Linear; };
    inline float GetQuadratic() const { return m_Quadratic; };

    void SetLightDir(glm::vec3 lightDir);
    void SetCutOff(float cutOff);
    void SetOuterCutOff(float outerCutOff);
    void SetConstant(float constant);
    void SetLinear(float linear);
    void SetQuadratic(float quadratic);

    void Bind(Shader& shader, glm::mat4 globalTransform) override;
    void Update(float deltaTime) override;

    glm::mat4 ComputeLightSpaceMatrix(glm::vec3 sceneCenter);
};