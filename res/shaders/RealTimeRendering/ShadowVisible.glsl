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
    vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // 世界空间位置
    vs_out.Normal = mat3(transpose(inverse(u_Model))) * a_Normal; //世界空间法线向量
    FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.TexCoords = a_TexCoords;
    vs_out.Tangent = a_Tangent;
    vs_out.Bitangent = a_Bitangent;
}

#shader fragment
#version 450 core
out vec4 FragColor;

in VS_OUT {//顶点着色器输出
    vec3 FragPos;//片元位置，世界空间
    vec2 TexCoords;//纹理坐标
    vec3 Tangent;//切线
    vec3 Bitangent;//副切线
    vec3 Normal;//片元法线，世界空间
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

// 材质属性
uniform bool hasNormalMap;
uniform bool hasEmissionMap;
uniform bool hasHeightMap;
uniform bool hasAlbedoMap;
uniform bool hasAO;
uniform bool hasMetallicMap;
uniform bool hasRoughnessMap;
uniform bool hasDissolveTextureMap;
uniform bool hasSpecularExponentTextureMap;

// 纹理采样器
uniform sampler2D AlbedoMap;       // 漫反射贴图         slot 0
uniform sampler2D NormalMap;       // 法线贴图              slot1               
uniform sampler2D MetallicMap;     // 金属度贴图         slot2
uniform sampler2D RoughnessMap;    // 粗糙度贴图         slot3
uniform sampler2D AOMap;           // 环境遮蔽贴图        slot4
uniform sampler2D EmissionMap;     // 自发光贴图         slot5
uniform sampler2D HeightMap;       // 高度贴图（可选）      slot6
uniform sampler2D ShadowMap;       // 阴影贴图              slot7
uniform sampler2D DissolveTextureMap; //透明度贴图           slot8 
uniform sampler2D SpecularExponentTextureMap; //镜面指数贴图  slot9
uniform sampler2D ViewDepthMap; //视觉深度贴图 slot 10
uniform samplerCube PointShadowMap; //点光源阴影贴图 slot 11

// 光源结构体
struct DirectionalLight
{
    vec3 lightDir;       // 方向光方向
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
};
uniform DirectionalLight directionalLight;

#define MAX_POINT_LIGHTS 4
struct PointLight
{
    vec3 lightPos;
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
    float constant;      // 常数衰减
    float linear;        // 线性衰减
    float quadratic;     // 二次方衰减
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

uniform vec3 viewPos;        // 观察者位置
in vec4 FragPosLightSpace;  // 片段着色器接受的光源空间位置

// 常量
const float PI = 3.14159265359;


bool IsVisible(vec3 fragPos);
float ShadowCalculation(vec4 fragPosLightSpace);
vec3 CalculateAmbientColor(vec3 albedo, vec3 ambientLight, float AO);
vec3 CalculateDiffuseColor(vec3 albedo, vec3 lightDiffuse, vec3 lightDir, vec3 normal);
vec3 CalculateSpecularColor(vec3 albedo, vec3 lightSpecular, vec3 lightDir, vec3 normal, vec3 viewDir, float roughness, float metallic);
float PointShadowCalculation(vec3 fragPos, vec3 lightPos);
// 主函数
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

// 几何遮蔽函数：Schlick-GGX
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// 几何遮蔽函数：Smith
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Fresnel 方程：Schlick近似
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
float ShadowCalculation(vec4 fragPosLightSpace)//使用PCF技术
{
    // 透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到 [0,1] 范围
    projCoords = projCoords * 0.5 + 0.5;
    // 获取当前片段的深度值
    float closestDepth = texture(ShadowMap, projCoords.xy).r;
    // 获取当前片段在光空间的深度值
    float currentDepth = projCoords.z;
    // 检查是否在阴影中
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);
    // 使用 3x3 样本
    for(int x = -sampleRate; x <= sampleRate; ++x)
    {
        for(int y = -sampleRate; y <= sampleRate; ++y)
        {
            float pcfDepth = texture(ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= sampleRate*sampleRate*4;
    
    // 如果超出了光照正交投影范围，则不在阴影中
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

float PointShadowCalculation(vec3 fragPos, vec3 lightPos) { //PCF
    vec3 lightToFragment = fragPos - lightPos;
    float distance = length(lightToFragment);

    // 采样点的方向（立方体贴图需要归一化方向向量）
    vec3 sampleDir = normalize(lightToFragment);
    float currentDepth = distance; // 当前片段的深度值（与光源的距离）

    // 初始化阴影值
    float shadow = 0.0;

    // PCF 采样参数
    float bias = 0.005; // 深度偏移，避免阴影失真
    int samples = 4;   // 采样次数，值越高阴影越柔和，性能开销也越大
    float offset = 0.05; // 偏移范围（柔和程度）

    for (int x = -samples / 2; x <= samples / 2; ++x) {
        for (int y = -samples / 2; y <= samples / 2; ++y) {
            for (int z = -samples / 2; z <= samples / 2; ++z) {
                // 生成偏移方向
                vec3 sampleOffset = vec3(float(x), float(y), float(z)) * offset;
                vec3 sampleDirOffset = normalize(sampleDir + sampleOffset);

                // 采样立方体贴图深度值
                float closestDepth = texture(PointShadowMap, sampleDirOffset).r * farPlane;

                // 如果当前深度大于最近深度，则在阴影中
                if (currentDepth - bias > closestDepth) {
                    shadow += 1.0;
                }
            }
        }
    }

    // 归一化阴影值（根据总采样数计算平均值）
    float totalSamples = pow(samples, 3);
    shadow /= totalSamples;

    // 返回最终阴影值（0 表示完全明亮，1 表示完全在阴影中）
    return shadow;
}

bool IsVisible(vec3 fragPos){
    vec3 projCoords = fragPos;
    // 获取当前片段的深度值
    float closestDepth = texture(ViewDepthMap, projCoords.xy).r;
    // 获取当前片段在光空间的深度值
    float currentDepth = projCoords.z;
    return closestDepth == currentDepth;
}