#shader vertex
#version 450 core

#include "layout.glsl"

uniform mat4 u_MVP;
uniform mat4 u_Model;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    flat int Slots[8];
    mat3 TBN;
} vs_out;

void main()
{
    gl_Position = u_MVP * a_Position;
    vs_out.FragPos = vec3(a_Position); // 世界空间位置
    vs_out.Normal = a_Normal.xyz; //世界空间法线向量
    vs_out.TexCoords = a_TexCoords.xy;
    vs_out.Tangent = a_Tangent.xyz;
    vs_out.Bitangent = a_Bitangent.xyz;
    vs_out.Slots[0] = int(a_Slot0);
    vs_out.Slots[1] = int(a_Slot1);
    vs_out.Slots[2] = int(a_Slot2);
    vs_out.Slots[3] = int(a_Slot3);
    vs_out.Slots[4] = int(a_Slot4);
    vs_out.Slots[5] = int(a_Slot5);
    vs_out.Slots[6] = int(a_Slot6);

    vec3 T = normalize(vec3(a_Tangent));
    vec3 B = normalize(vec3(a_Bitangent));
    vec3 N = normalize(vec3(a_Normal));

    // 构建 TBN 矩阵
    vs_out.TBN = mat3(T, B, N);
}

#shader fragment
#version 450 core
out vec4 FragColor;

in VS_OUT {//顶点着色器输出
    vec3 FragPos;//片元位置，世界空间
    vec2 TexCoords;//纹理坐标
    vec3 Normal;//片元法线，世界空间
    vec3 Tangent;
    vec3 Bitangent;
    flat int Slots[8];
    mat3 TBN;
} fs_in;


uniform sampler2DArray textures;

uniform float farPlane;
uniform vec3 Diffuse;
uniform vec3 Emission;

#define MAX_DIRECTIONAL_LIGHTS 1
// 光源结构体
struct DirectionalLight
{
    vec3 lightDir;       // 方向光方向
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
};
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform int numDirectionalLights;

#define MAX_POINT_LIGHTS 3
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

uniform vec3 viewPos;        

// 常量
const float PI = 3.14159265359;


bool IsVisible(vec3 fragPos);
vec3 CalculateAmbientColor(vec3 albedo, vec3 ambientLight, float AO);
vec3 CalculateDiffuseColor(vec3 albedo, vec3 lightDiffuse, vec3 lightDir, vec3 normal);
vec3 CalculateSpecularColor(vec3 albedo, vec3 lightSpecular, vec3 lightDir, vec3 normal, vec3 viewDir, float roughness, float metallic);

vec3 GetTextureColor(int slot, vec2 texCoords) {
    return texture(textures, vec3(texCoords, float(slot))).rgb;
}

float GetTextureValue(int slot, vec2 texCoords) {
    return texture(textures, vec3(texCoords, float(slot))).r;
}

uniform float focusDepth;
uniform float focusRange;
// 主函数
void main()
{
    vec3 finalColor = vec3(0.0);
    //计算颜色
    vec3 ambient = fs_in.Slots[0] != -1 ? GetTextureColor(fs_in.Slots[0], fs_in.TexCoords) : Diffuse;// 本来的颜色 0
    vec3 diffuse = ambient;
    vec3 specular = ambient;
    //计算法线
    vec3 normal = vec3(0.0);
    if( fs_in.Slots[1] != -1){
        vec3 sampledNormal =  GetTextureColor(fs_in.Slots[1], fs_in.TexCoords); 
        sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
        normal = normalize(fs_in.TBN * sampledNormal);
    }
    else{
        normal = normalize(fs_in.Normal);
    }
    //计算其他不是很重要的参数
    float metallic = fs_in.Slots[2] != -1 ? GetTextureValue(fs_in.Slots[2], fs_in.TexCoords) : 0.0; // 金属度 2
    float roughness = fs_in.Slots[3] != -1 ? GetTextureValue(fs_in.Slots[3], fs_in.TexCoords) : 0.5; // 粗糙度 3
    float AO = fs_in.Slots[4] != -1 ? GetTextureValue(fs_in.Slots[4], fs_in.TexCoords) : 1.0; // 环境光遮蔽 4
    vec3 emission = fs_in.Slots[5] != -1 ? GetTextureColor(fs_in.Slots[5], fs_in.TexCoords) : Emission; // 自发光 5
    float alpha = fs_in.Slots[6] != -1 ? GetTextureValue(fs_in.Slots[6], fs_in.TexCoords) : 1.0;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    for(int i = 0; i < numDirectionalLights; i++)
    {
        vec3 ambientColor = CalculateAmbientColor(ambient, directionalLights[i].lightAmbient, AO);
        vec3 diffuseColor = CalculateDiffuseColor(diffuse, directionalLights[i].lightDiffuse, directionalLights[i].lightDir, normal);
        vec3 specularColor = CalculateSpecularColor(specular, directionalLights[i].lightSpecular, directionalLights[i].lightDir, normal, viewDir, roughness, metallic);
        finalColor += ambientColor + diffuseColor + specularColor + emission;
    }

    for(int i = 0; i < numPointLights; i++)
    {
        vec3 lightDir = pointLights[i].lightPos - fs_in.FragPos;
        vec3 pointAmbient = CalculateAmbientColor(ambient, pointLights[i].lightAmbient, AO);
        vec3 pointDiffuse = CalculateDiffuseColor(diffuse, pointLights[i].lightDiffuse, lightDir, normal);
        vec3 pointSpecular = CalculateSpecularColor(specular, pointLights[i].lightSpecular, lightDir, normal, viewDir, roughness, metallic);
        float distance = distance(fs_in.FragPos, pointLights[i].lightPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
        pointAmbient = pointAmbient * attenuation;
        pointDiffuse = pointDiffuse * attenuation;
        pointSpecular = pointSpecular * attenuation;
        finalColor += pointAmbient + pointDiffuse + pointSpecular;
    }

    if( alpha == 0){
        discard;
    }
    FragColor = vec4(finalColor, 1.0);
    if( abs(gl_FragCoord.z - focusDepth) <= focusRange){
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
}

vec3 CalculateAmbientColor(vec3 ambient, vec3 lightAmbient, float AO){
    //ambient color
    vec3 ambientColor = lightAmbient * ambient * AO;
    return ambientColor;
}
vec3 CalculateDiffuseColor(vec3 diffuse, vec3 lightDiffuse, vec3 lightDir, vec3 normal){
    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
    vec3 diffuseColor = lightDiffuse * diffuse * max(dot(lightDir, normal), 0);
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
