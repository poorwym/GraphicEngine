#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    vec3 Normal;
} vs_out;
out vec4 FragPosLightSpace;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1.0);
    vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // ����ռ�λ��
    vs_out.Normal = mat3(transpose(inverse(u_Model))) * a_Normal; //����ռ䷨������
    FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.TexCoords = a_TexCoords;
    vs_out.Tangent = a_Tangent;
    vs_out.Bitangent = a_Bitangent;
}

#shader fragment
#version 450 core
out vec4 FragColor;

in VS_OUT {//������ɫ�����
    vec3 FragPos;//ƬԪλ�ã�����ռ�
    vec2 TexCoords;//��������
    vec3 Tangent;//����
    vec3 Bitangent;//������
    vec3 Normal;//ƬԪ���ߣ�����ռ�
} fs_in;

uniform float farPlane;
uniform vec3 Ambient;
uniform vec3 Diffuse;
uniform vec3 Specular;
uniform vec3 Transmittance;
uniform vec3 Emission;
uniform float Shininess;
uniform float Ior;
uniform float d;
uniform int Illum;

// ��������
uniform bool hasNormalMap;
uniform bool hasEmissionMap;
uniform bool hasHeightMap;
uniform bool hasAlbedoMap;
uniform bool hasAO;
uniform bool hasMetallicMap;
uniform bool hasRoughnessMap;
uniform bool hasDissolveTextureMap;
uniform bool hasSpecularExponentTextureMap;

// ���������
uniform sampler2D AlbedoMap;       // ��������ͼ         slot 0
uniform sampler2D NormalMap;       // ������ͼ              slot1               
uniform sampler2D MetallicMap;     // ��������ͼ         slot2
uniform sampler2D RoughnessMap;    // �ֲڶ���ͼ         slot3
uniform sampler2D AOMap;           // �����ڱ���ͼ        slot4
uniform sampler2D EmissionMap;     // �Է�����ͼ         slot5
uniform sampler2D HeightMap;       // �߶���ͼ����ѡ��      slot6
uniform sampler2D ShadowMap;       // ��Ӱ��ͼ              slot7
uniform sampler2D DissolveTextureMap; //͸������ͼ           slot8 
uniform sampler2D SpecularExponentTextureMap; //����ָ����ͼ  slot9
uniform sampler2D ViewDepthMap; //�Ӿ������ͼ slot 10
uniform samplerCube PointShadowMap; //���Դ��Ӱ��ͼ slot 11

// ��Դ�ṹ��
struct DirectionalLight
{
    vec3 lightDir;       // ����ⷽ��
    vec3 lightAmbient;   // ������ǿ��
    vec3 lightDiffuse;   // �������ǿ��
    vec3 lightSpecular;  // �����ǿ��
};
uniform DirectionalLight directionalLight;

#define MAX_POINT_LIGHTS 4
struct PointLight
{
    vec3 lightPos;
    vec3 lightAmbient;   // ������ǿ��
    vec3 lightDiffuse;   // �������ǿ��
    vec3 lightSpecular;  // �����ǿ��
    float constant;      // ����˥��
    float linear;        // ����˥��
    float quadratic;     // ���η�˥��
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

uniform vec3 viewPos;        // �۲���λ��
in vec4 FragPosLightSpace;  // Ƭ����ɫ�����ܵĹ�Դ�ռ�λ��

// ����
const float PI = 3.14159265359;


bool IsVisible(vec3 fragPos);
float ShadowCalculation(vec4 fragPosLightSpace);
vec3 CalculateAmbientColor(vec3 albedo, vec3 ambientLight, float AO);
vec3 CalculateDiffuseColor(vec3 albedo, vec3 lightDiffuse, vec3 lightDir, vec3 normal);
vec3 CalculateSpecularColor(vec3 albedo, vec3 lightSpecular, vec3 lightDir, vec3 normal, vec3 viewDir, float roughness, float metallic);
float PointShadowCalculation(vec3 fragPos, vec3 lightPos);
// ������
void main()
{  
    vec3 finalColor = vec3(1.0);
    float shadow = ShadowCalculation(lightSpaceMatrix);
    if(IsVisible(fs_in.FragPos)) FragColor = vec4(vec3(shadow), 1.0);
}

vec3 CalculateAmbientColor(vec3 ambient, vec3 lightAmbient, float AO){
    //ambient color
    vec3 ambientColor = lightAmbient * ambient * AO;
    return ambientColor;
}
vec3 CalculateDiffuseColor(vec3 diffuse, vec3 lightDiffuse, vec3 lightDir, vec3 normal){
    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
    vec3 diffuseColor = lightDiffuse * diffuse * max(N * L, 0);
    return diffuseColor;
}

// �����ڱκ�����Schlick-GGX
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// �����ڱκ�����Smith
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Fresnel ���̣�Schlick����
vec3 fresnelSchlick(float cosTheta, float metallic, vec3 albedo)
{
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float GGXDistribution(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
vec3 CalculateSpecularColor(vec3 specular, vec3 lightSpecular, vec3 lightDir, vec3 normal, vec3 viewDir, float roughness, float metallic){
    vec3 V = normalize(viewDir);
    vec3 L = normalize(lightDir);
    vec3 H = normalize(V + L);
    vec3 N = normalize(normal);
    vec3 F = fresnelSchlick(abs(dot(H, V)), metallic, specular);
    float G = GeometrySmith(N, V, L, roughness);
    float D = GGXDistribution(N, H, roughness);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    vec3 specularColor = lightSpecular * F * D * G / max(4.0 * NdotL * NdotV, 0.001);
    return specularColor;
}

const int sampleRate = 5;
const float bias = 0.0005;
float ShadowCalculation(vec4 fragPosLightSpace)//ʹ��PCF����
{
    // ͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // �任�� [0,1] ��Χ
    projCoords = projCoords * 0.5 + 0.5;
    // ��ȡ��ǰƬ�ε����ֵ
    float closestDepth = texture(ShadowMap, projCoords.xy).r;
    // ��ȡ��ǰƬ���ڹ�ռ�����ֵ
    float currentDepth = projCoords.z;
    // ����Ƿ�����Ӱ��
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);
    // ʹ�� 3x3 ����
    for(int x = -sampleRate; x <= sampleRate; ++x)
    {
        for(int y = -sampleRate; y <= sampleRate; ++y)
        {
            float pcfDepth = texture(ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= sampleRate*sampleRate*4;
    
    // ��������˹�������ͶӰ��Χ��������Ӱ��
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

float PointShadowCalculation(vec3 fragPos, vec3 lightPos) { //PCF
    vec3 lightToFragment = fragPos - lightPos;
    float distance = length(lightToFragment);

    // ������ķ�����������ͼ��Ҫ��һ������������
    vec3 sampleDir = normalize(lightToFragment);
    float currentDepth = distance; // ��ǰƬ�ε����ֵ�����Դ�ľ��룩

    // ��ʼ����Ӱֵ
    float shadow = 0.0;

    // PCF ��������
    float bias = 0.005; // ���ƫ�ƣ�������Ӱʧ��
    int samples = 4;   // ����������ֵԽ����ӰԽ��ͣ����ܿ���ҲԽ��
    float offset = 0.05; // ƫ�Ʒ�Χ����ͳ̶ȣ�

    for (int x = -samples / 2; x <= samples / 2; ++x) {
        for (int y = -samples / 2; y <= samples / 2; ++y) {
            for (int z = -samples / 2; z <= samples / 2; ++z) {
                // ����ƫ�Ʒ���
                vec3 sampleOffset = vec3(float(x), float(y), float(z)) * offset;
                vec3 sampleDirOffset = normalize(sampleDir + sampleOffset);

                // ������������ͼ���ֵ
                float closestDepth = texture(PointShadowMap, sampleDirOffset).r * farPlane;

                // �����ǰ��ȴ��������ȣ�������Ӱ��
                if (currentDepth - bias > closestDepth) {
                    shadow += 1.0;
                }
            }
        }
    }

    // ��һ����Ӱֵ�������ܲ���������ƽ��ֵ��
    float totalSamples = pow(samples, 3);
    shadow /= totalSamples;

    // ����������Ӱֵ��0 ��ʾ��ȫ������1 ��ʾ��ȫ����Ӱ�У�
    return shadow;
}

bool IsVisible(vec3 fragPos){
    vec3 projCoords = fragPos;
    // ��ȡ��ǰƬ�ε����ֵ
    float closestDepth = texture(ViewDepthMap, projCoords.xy).r;
    // ��ȡ��ǰƬ���ڹ�ռ�����ֵ
    float currentDepth = projCoords.z;
    return closestDepth == currentDepth;
}