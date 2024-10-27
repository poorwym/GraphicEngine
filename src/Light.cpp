#include "Light.h"
#include "SceneManager.h"
DirectionalLight::DirectionalLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightDir, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular)
	: Light(name, color, intensity)
{
	m_Name = name;
	m_LightDir = lightDir;
	m_LightAmbient = m_Intensity * lightAmbient;
	m_LightDiffuse = m_Intensity * lightDiffuse;
	m_LightSpecular = m_Intensity * lightSpecular;
}
DirectionalLight::DirectionalLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightDir)
	: Light(name, color, intensity)
{
	m_LightDir = lightDir;
}
/*uniform vec3 lightDir;       // ����ⷽ��
uniform vec3 lightAmbient;   // ������ǿ��
uniform vec3 lightDiffuse;   // �������ǿ��
uniform vec3 lightSpecular;  // �����ǿ��*/
void DirectionalLight::SetLightDir(glm::vec3 lightDir)
{
	m_LightDir = lightDir;
}
void Light::SetLightAmbient(glm::vec3 lightAmbient)
{
	m_LightAmbient = lightAmbient;
}
void Light::SetLightDiffuse(glm::vec3 lightDiffuse)
{
	m_LightDiffuse = lightDiffuse;
}
void Light::SetLightSpecular(glm::vec3 lightSpecular)
{
	m_LightSpecular = lightSpecular;
}
void DirectionalLight::Bind(Shader& shader, glm::mat4 globalTransform)
{
	shader.setUniformVec3f("directionalLight.lightDir", m_LightDir);
	shader.setUniformVec3f("directionalLight.lightAmbient", m_LightAmbient);
	shader.setUniformVec3f("directionalLight.lightDiffuse", m_LightDiffuse);
	shader.setUniformVec3f("directionalLight.lightSpecular", m_LightSpecular);
}

void DirectionalLight::Update(float deltaTime)
{
	m_LightAmbient = m_Intensity * m_Color;
	m_LightDiffuse = m_Intensity * m_Color;
	m_LightSpecular = m_Intensity * m_Color;
}

Light::Light(const std::string& name, glm::vec3 color, float intensity)
	:m_Name(name), m_Color(color), m_Intensity(intensity)
{
	m_LightAmbient = m_Intensity * color;
	m_LightDiffuse = m_Intensity * color;
	m_LightSpecular = m_Intensity * color;
}

void Light::SetLightColor(glm::vec3 color)
{
	m_Color = color;
}

void Light::SetLightIntensity(float intensity)
{
	m_Intensity = intensity;
}

void Light::Update(float deltaTime)
{
}

PointLight::PointLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightPos)
	: Light(name, color, intensity)
{
	m_LightPos = lightPos;
	m_LightAmbient = m_Intensity * color;
	m_LightDiffuse = m_Intensity * color;
	m_LightSpecular = m_Intensity * color;
	m_Constant = 1.0f;
	m_Linear = 1.0f;
	m_Quadratic = 1.0f;
}

PointLight::PointLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightPos, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular, float constant, float linear, float quadratic)
	: Light(name, color, intensity)
{
	m_LightPos = lightPos;
	m_LightAmbient = m_Intensity * lightAmbient;
	m_LightDiffuse = m_Intensity * lightDiffuse;
	m_LightSpecular = m_Intensity * lightSpecular;
	m_Constant = constant;
	m_Linear = linear;
	m_Quadratic = quadratic;
}

void PointLight::SetLightPos(glm::vec3 lightPos)
{
	m_LightPos = lightPos;
}

void PointLight::SetConstant(float constant)
{
	m_Constant = constant;
}

void PointLight::SetLinear(float linear)
{
	m_Linear = linear;
}

void PointLight::SetQuadratic(float quadratic)
{
	m_Quadratic = quadratic;
}

void PointLight::Bind(Shader& shader, glm::mat4 globalTransform)
{
	glm::vec3 position = globalTransform * glm::vec4(m_LightPos, 1.0f);
	std::string number = std::to_string(pointLightID[this]);
	shader.setUniformVec3f("pointLights[" + number + "].lightPos", position);
	shader.setUniformVec3f("pointLights[" + number + "].lightAmbient", m_LightAmbient);
	shader.setUniformVec3f("pointLights[" + number + "].lightDiffuse", m_LightDiffuse);
	shader.setUniformVec3f("pointLights[" + number + "].lightSpecular", m_LightSpecular);
	shader.setUniform1f("pointLights[" + number + "].constant", m_Constant);
	shader.setUniform1f("pointLights[" + number + "].linear", m_Linear);
	shader.setUniform1f("pointLights[" + number + "].quadratic", m_Quadratic);
}

void PointLight::Update(float deltaTime)
{
	m_LightAmbient = m_Intensity * m_Color;
	m_LightDiffuse = m_Intensity * m_Color;
	m_LightSpecular = m_Intensity * m_Color;
}
