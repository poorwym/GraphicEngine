#include "Light.h"
DirectionalLight::DirectionalLight(glm::vec3 color, float intensity, glm::vec3 lightDir, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular)
	: Light(color, intensity)
{
	m_LightDir = lightDir;
	m_LightAmbient = m_Intensity * lightAmbient;
	m_LightDiffuse = m_Intensity * lightDiffuse;
	m_LightSpecular = m_Intensity * lightSpecular;
}
DirectionalLight::DirectionalLight(glm::vec3 color, float intensity, glm::vec3 lightDir)
	: Light(color, intensity)
{
	m_LightDir = lightDir;
	
}
/*uniform vec3 lightDir;       // 方向光方向
uniform vec3 lightAmbient;   // 环境光强度
uniform vec3 lightDiffuse;   // 漫反射光强度
uniform vec3 lightSpecular;  // 镜面光强度*/
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
void DirectionalLight::Bind(Shader& shader)
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

Light::Light(glm::vec3 color, float intensity)
	:m_Color(color), m_Intensity(intensity)
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

PointLight::PointLight(glm::vec3 color, float intensity, glm::vec3 lightPos)
	: Light(color, intensity)
{
	m_LightPos = lightPos;
	m_LightAmbient = m_Intensity * color;
	m_LightDiffuse = m_Intensity * color;
	m_LightSpecular = m_Intensity * color;
	m_Constant = 1.0f;
	m_Linear = 1.0f;
	m_Quadratic = 1.0f;
}

PointLight::PointLight(glm::vec3 color, float intensity, glm::vec3 lightPos, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular, float constant, float linear, float quadratic)
	: Light(color, intensity)
{
	m_LightPos = lightPos;
	m_LightAmbient = m_Intensity * lightAmbient;
	m_LightDiffuse = m_Intensity * lightDiffuse;
	m_LightSpecular = m_Intensity * lightSpecular;
	m_Constant = constant;
	m_Linear = linear;
	m_Quadratic = quadratic;
}

void PointLight::Bind(Shader& shader)
{
}
