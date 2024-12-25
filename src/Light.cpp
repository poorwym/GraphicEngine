#include "Light.h"
#include "SceneManager.h"
#include "Macro.h"

extern std::map<std::string, PointLight*> g_PointLightList;
extern std::map<PointLight*, unsigned int> g_PointLightID;
extern std::map<std::string, SpotLight*> g_SpotLightList;
extern std::map<SpotLight*, unsigned int> g_SpotLightID;
extern std::map<std::string, DirectionalLight*> g_DirectionalLightList;
extern std::map<DirectionalLight*, unsigned int> g_DirectionalLightID;



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
	g_DirectionalLightList[name] = this;
    g_DirectionalLightID[this] = g_DirectionalLightList.size() - 1;
}

DirectionalLight::~DirectionalLight()
{
    g_DirectionalLightList.erase(m_Name);
    g_DirectionalLightID.erase(this);
}


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
	std::string number = std::to_string(g_DirectionalLightID[this]);
	shader.SetUniformVec3f("directionalLights["+number+"].lightDir", m_LightDir);
	shader.SetUniformVec3f("directionalLights["+number+"].lightAmbient", m_LightAmbient);
	shader.SetUniformVec3f("directionalLights["+number+"].lightDiffuse", m_LightDiffuse);
	shader.SetUniformVec3f("directionalLights["+number+"].lightSpecular", m_LightSpecular);
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
	float sceneRadius = 200.0f; // ���ݳ�����ģ����
	float distanceFromScene = sceneRadius * 2.0f;
	glm::vec3 lightPos = sceneCenter - lightDir * distanceFromScene;

	// ѡ����������������ⷽ��ƽ��
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
	m_Constant = 1.0f;
	m_Linear = 0.0f;
	m_Quadratic = 0.0f;
	g_PointLightList[name] = this;
	g_PointLightID[this] = g_PointLightList.size() - 1;
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
	g_PointLightList[name] = this;
	g_PointLightID[this] = g_PointLightList.size() - 1;
}

PointLight::~PointLight()
{
    g_PointLightList.erase(m_Name);
    g_PointLightID.erase(this);
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
	shader.Bind();
	glm::vec3 position = globalTransform * glm::vec4(m_LightPos, 1.0f);
	std::string number = std::to_string(g_PointLightID[this]);
	shader.SetUniformVec3f("pointLights[" + number + "].lightPos", position);
	shader.SetUniformVec3f("pointLights[" + number + "].lightAmbient", m_LightAmbient);
	shader.SetUniformVec3f("pointLights[" + number + "].lightDiffuse", m_LightDiffuse);
	shader.SetUniformVec3f("pointLights[" + number + "].lightSpecular", m_LightSpecular);
	shader.SetUniform1f("pointLights[" + number + "].constant", m_Constant);
	shader.SetUniform1f("pointLights[" + number + "].linear", m_Linear);
	shader.SetUniform1f("pointLights[" + number + "].quadratic", m_Quadratic);
	shader.Unbind();
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

SpotLight::SpotLight(const std::string& name, glm::vec3 color, float intensity,
	glm::vec3 lightPos, glm::vec3 lightDir,
	float cutOff, float outerCutOff)
	: Light(name, color, intensity), m_ShadowMapFBO(nullptr)
{
	m_LightPos = lightPos;
	m_LightDir = glm::normalize(lightDir);
	m_CutOff = glm::cos(glm::radians(cutOff));
	m_OuterCutOff = glm::cos(glm::radians(outerCutOff));

	m_LightAmbient = m_Intensity * m_AmbientColor;
	m_LightDiffuse = m_Intensity * m_DiffuseColor;
	m_LightSpecular = m_Intensity * m_SpecularColor;

	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;

	// ��ʼ����Ӱӳ��FBO����ѡ��
	m_ShadowMapFBO = new DepthMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);

    g_SpotLightList[name] = this;
    g_SpotLightID[this] = g_SpotLightList.size() - 1;
}

SpotLight::SpotLight(const std::string& name, glm::vec3 color, float intensity,
	glm::vec3 lightPos, glm::vec3 lightDir,
	glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular,
	float constant, float linear, float quadratic,
	float cutOff, float outerCutOff)
	: Light(name, color, intensity), m_ShadowMapFBO(nullptr)
{
	m_LightPos = lightPos;
	m_LightDir = glm::normalize(lightDir);
	m_CutOff = glm::cos(glm::radians(cutOff));
	m_OuterCutOff = glm::cos(glm::radians(outerCutOff));

	m_LightAmbient = m_Intensity * lightAmbient;
	m_LightDiffuse = m_Intensity * lightDiffuse;
	m_LightSpecular = m_Intensity * lightSpecular;

	m_Constant = constant;
	m_Linear = linear;
	m_Quadratic = quadratic;

	// ��ʼ����Ӱӳ��FBO����ѡ��
	m_ShadowMapFBO = new DepthMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);

    g_SpotLightList[name] = this;
    g_SpotLightID[this] = g_SpotLightList.size() - 1;
}

SpotLight::~SpotLight()
{
    g_SpotLightList.erase(m_Name);
    g_SpotLightID.erase(this);
}

// ���úͻ�ȡ����
void SpotLight::SetLightDir(glm::vec3 lightDir)
{
	m_LightDir = glm::normalize(lightDir);
}

// ���úͻ�ȡ׶��
void SpotLight::SetCutOff(float cutOff)
{
	m_CutOff = glm::cos(glm::radians(cutOff));
}

void SpotLight::SetOuterCutOff(float outerCutOff)
{
	m_OuterCutOff = glm::cos(glm::radians(outerCutOff));
}

// ���úͻ�ȡ˥������
void SpotLight::SetConstant(float constant)
{
	m_Constant = constant;
}

void SpotLight::SetLinear(float linear)
{
	m_Linear = linear;
}

void SpotLight::SetQuadratic(float quadratic)
{
	m_Quadratic = quadratic;
}

// �󶨹�Դ����ɫ��
void SpotLight::Bind(Shader& shader, glm::mat4 globalTransform)
{
	shader.Bind();
	glm::vec3 position = glm::vec3(globalTransform * glm::vec4(m_LightPos, 1.0f));
	glm::vec3 direction = glm::normalize(glm::vec3(globalTransform * glm::vec4(m_LightDir, 0.0f)));

	std::string number = std::to_string(g_SpotLightID[this]);
	shader.SetUniformVec3f("spotLights[" + number + "].lightPos", position);
	shader.SetUniformVec3f("spotLights[" + number + "].lightDir", direction);
	shader.SetUniformVec3f("spotLights[" + number + "].lightAmbient", m_LightAmbient);
	shader.SetUniformVec3f("spotLights[" + number + "].lightDiffuse", m_LightDiffuse);
	shader.SetUniformVec3f("spotLights[" + number + "].lightSpecular", m_LightSpecular);
	shader.SetUniform1f("spotLights[" + number + "].constant", m_Constant);
	shader.SetUniform1f("spotLights[" + number + "].linear", m_Linear);
	shader.SetUniform1f("spotLights[" + number + "].quadratic", m_Quadratic);
	shader.SetUniform1f("spotLights[" + number + "].cutOff", m_CutOff);
	shader.SetUniform1f("spotLights[" + number + "].outerCutOff", m_OuterCutOff);
	shader.Unbind();
}

// ���¹�Դ����
void SpotLight::Update(float deltaTime)
{
	m_LightAmbient = m_Intensity * m_AmbientColor;
	m_LightDiffuse = m_Intensity * m_DiffuseColor;
	m_LightSpecular = m_Intensity * m_SpecularColor;
}

glm::mat4 SpotLight::ComputeLightSpaceMatrix(glm::vec3 sceneCenter)
{
	float nearPlane = 1.0f;
	float farPlane = 25.0f;
	glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, nearPlane, farPlane);
	glm::mat4 lightView = glm::lookAt(m_LightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
	return lightProjection * lightView;
}