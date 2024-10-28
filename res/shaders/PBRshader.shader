#shader vertex
#version 330 core

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
#version 330 core
out vec4 FragColor;

in VS_OUT {//顶点着色器输出
    vec3 FragPos;//片元位置，世界空间
    vec2 TexCoords;//纹理坐标
    vec3 Tangent;//切线
    vec3 Bitangent;//副切线
    vec3 Normal;//片元法线，世界空间
} fs_in;

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



float ShadowCalculation(vec4 fragPosLightSpace);
// 主函数
void main()
{
    vec3 AmbientColor = texture(AlbedoMap, fs_in.TexCoords).rgb;
    float dirShadow = ShadowCalculation(FragPosLightSpace);
    FragColor = vec4(AmbientColor * (1 - dirShadow) ,1.0);
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
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

const int sampleRate = 5;
const float bias = 0.0005;
float ShadowCalculation(vec4 fragPosLightSpace)
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