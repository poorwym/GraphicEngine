#include "Light.h"
#include "SceneManager.h"
#include "Macro.h"
DirectionalLight::DirectionalLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightDir, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular)
	: Light(name, color, intensity), m_ShadowMapFBO(nullptr)
{
	m_ShadowMapFBO = new DepthMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);
	m_LightDir = lightDir;
	m_AmbientColor = lightAmbient;
	m_DiffuseColor = lightDiffuse;
    m_SpecularColor = lightSpecular;
}
DirectionalLight::DirectionalLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightDir)
	: Light(name, color, intensity), m_ShadowMapFBO(nullptr)
{
	m_LightDir = lightDir;
	m_ShadowMapFBO = new DepthMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);
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
void DirectionalLight::Bind(Shader& shader, glm::mat4 globalTransform)
{
	shader.setUniformVec3f("directionalLight.lightDir", m_LightDir);
	shader.setUniformVec3f("directionalLight.lightAmbient", m_LightAmbient);
	shader.setUniformVec3f("directionalLight.lightDiffuse", m_LightDiffuse);
	shader.setUniformVec3f("directionalLight.lightSpecular", m_LightSpecular);
}

void DirectionalLight::Update(float deltaTime)
{
	m_LightAmbient = m_Color;
	m_LightDiffuse = m_Intensity * m_Color;
	m_LightSpecular = m_Intensity * m_Color;
}

glm::mat4 DirectionalLight::ComputeLightSpaceMatrix(glm::vec3 sceneCenter)
{
	glm::vec3 lightDir = m_LightDir;
	float sceneRadius = 200.0f; // 根据场景规模调整
	float distanceFromScene = sceneRadius * 2.0f;
	glm::vec3 lightPos = sceneCenter - lightDir * distanceFromScene;

	// 选择上向量，避免与光方向平行
	glm::vec3 up = (glm::abs(lightDir.x) < 0.001f && glm::abs(lightDir.z) < 0.001f)
		? glm::vec3(0.0f, 0.0f, 1.0f)
		: glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, up);
	float orthoSize = sceneRadius * 2.0f;
	float nearPlane = 0.1f;
	float farPlane = distanceFromScene + sceneRadius * 2.0f;

	glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
	return lightProjection * lightView;
}

Light::Light(const std::string& name, glm::vec3 color, float intensity)
	:m_Name(name), m_Color(color), m_Intensity(intensity), m_AmbientColor(color), m_DiffuseColor(color), m_SpecularColor(color)
{
	m_LightAmbient = m_Intensity * m_AmbientColor;
	m_LightDiffuse = m_Intensity * m_DiffuseColor;
	m_LightSpecular = m_Intensity * m_SpecularColor;
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
	: Light(name, color, intensity),m_CubeShadowMapFBO(nullptr)
{
	m_CubeShadowMapFBO = new CubeMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);
	m_LightPos = lightPos;
	m_LightAmbient = m_Intensity * color;
	m_LightDiffuse = m_Intensity * color;
	m_LightSpecular = m_Intensity * color;
	m_Constant = 0.5f;
	m_Linear = 1.0f;
	m_Quadratic = 1.0f;
}

PointLight::PointLight(const std::string& name, glm::vec3 color, float intensity, glm::vec3 lightPos, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular, float constant, float linear, float quadratic)
	: Light(name, color, intensity), m_CubeShadowMapFBO(nullptr)
{
	m_CubeShadowMapFBO = new CubeMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);
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
	m_LightAmbient = m_Color;
	m_LightDiffuse = m_Intensity * m_Color;
	m_LightSpecular = m_Intensity * m_Color;
}

std::vector<glm::mat4> PointLight::ComputePointLightShadowMatrices(float nearPlane, float farPlane) {
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	return shadowTransforms;
}