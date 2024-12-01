#shader vertex
#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;
layout (location = 5) in float a_Slot0;
layout (location = 6) in float a_Slot1;
layout (location = 7) in float a_Slot2;
layout (location = 8) in float a_Slot3;
layout (location = 9) in float a_Slot4;
layout (location = 10) in float a_Slot5;
layout (location = 11) in float a_Slot6;
layout (location = 12) in float a_Slot7;

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
out vec4 FragPosLightSpace;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1.0);
    vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // 世界空间位置
    vs_out.Normal = a_Normal; //世界空间法线向量
    FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.TexCoords = a_TexCoords;
    vs_out.Tangent = a_Tangent;
    vs_out.Bitangent = a_Bitangent;
    vs_out.Slots[0] = int(a_Slot0);
    vs_out.Slots[1] = int(a_Slot1);
    vs_out.Slots[2] = int(a_Slot2);
    vs_out.Slots[3] = int(a_Slot3);
    vs_out.Slots[4] = int(a_Slot4);
    vs_out.Slots[5] = int(a_Slot5);
    vs_out.Slots[6] = int(a_Slot6);

    vec3 T = normalize(mat3(u_Model) * a_Tangent);
    vec3 B = normalize(mat3(u_Model) * a_Bitangent);
    vec3 N = normalize(mat3(u_Model) * a_Normal);

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

uniform sampler2D ShadowMap; //slot 31
uniform float farPlane;
uniform vec3 Diffuse;
uniform vec3 Emission;

// 光源结构体
struct DirectionalLight
{
    vec3 lightDir;       // 方向光方向
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
};
uniform DirectionalLight directionalLight;

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
uniform samplerCube PointShadowMap[3];// slot 27-30

uniform vec3 viewPos;        // 观察者位置
in vec4 FragPosLightSpace;  // 片段着色器接受的光源空间位置

// 常量
const float PI = 3.14159265359;


bool IsVisible(vec3 fragPos);
float ShadowCalculation(vec4 fragPosLightSpace);
vec3 CalculateAmbientColor(vec3 albedo, vec3 ambientLight, float AO);
vec3 CalculateDiffuseColor(vec3 albedo, vec3 lightDiffuse, vec3 lightDir, vec3 normal);
vec3 CalculateSpecularColor(vec3 albedo, vec3 lightSpecular, vec3 lightDir, vec3 normal, vec3 viewDir, float roughness, float metallic);
float PointShadowCalculation(vec3 fragPos, vec3 lightPos, int i);

vec3 GetTextureColor(int slot, vec2 texCoords) {
    return texture(textures, vec3(texCoords, float(slot))).rgb;
}

float GetTextureValue(int slot, vec2 texCoords) {
    return texture(textures, vec3(texCoords, float(slot))).r;
}

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
    vec3 ambientColor = CalculateAmbientColor(ambient, directionalLight.lightAmbient, AO);
    vec3 diffuseColor = CalculateDiffuseColor(diffuse, directionalLight.lightDiffuse, directionalLight.lightDir, normal);
    vec3 specularColor = CalculateSpecularColor(specular, directionalLight.lightSpecular, directionalLight.lightDir, normal, viewDir, roughness, metallic);
    float dirShadow = ShadowCalculation(FragPosLightSpace);

    finalColor += ambientColor + (1 - dirShadow) * (diffuseColor + specularColor) + emission;
    for(int i=0; i < numPointLights; i++)
    {
        vec3 lightDir = pointLights[i].lightPos - fs_in.FragPos;
        vec3 pointAmbient = CalculateAmbientColor(ambient, pointLights[i].lightAmbient, AO);
        vec3 pointDiffuse = CalculateDiffuseColor(diffuse, pointLights[i].lightDiffuse, lightDir, normal);
        vec3 pointSpecular = CalculateSpecularColor(specular, pointLights[i].lightSpecular, lightDir, normal, viewDir, roughness, metallic);
        float distance = distance(fs_in.FragPos, pointLights[i].lightPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
        float pointShadow = PointShadowCalculation(fs_in.FragPos, pointLights[i].lightPos, i);
        pointAmbient = pointAmbient * attenuation;
        pointDiffuse = pointDiffuse * attenuation;
        pointSpecular = pointSpecular * attenuation;
        finalColor += pointAmbient + (1 - pointShadow) * (pointDiffuse + pointSpecular);
    }
    if( alpha == 0){
        discard;
    }
    FragColor = vec4(finalColor, 1.0);
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

const int sampleRate = 10;
const float bias = 0.0000005;
float ShadowCalculation(vec4 fragPosLightSpace) //PCF
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

float PointShadowCalculation(vec3 fragPos, vec3 lightPos, int i) { //PCF
    vec3 lightToFragment = fragPos - lightPos;
    float distance = length(lightToFragment);

    // 采样点的方向（立方体贴图需要归一化方向向量）
    vec3 sampleDir = normalize(lightToFragment);
    float currentDepth = distance; // 当前片段的深度值（与光源的距离）

    // 初始化阴影值
    float shadow = 0.0;

    // PCF 采样参数
    float bias = 0.005; // 深度偏移，避免阴影失真
    int samples = 8;   // 采样次数，值越高阴影越柔和，性能开销也越大
    float offset = 0.00000005; // 偏移范围（柔和程度）

    for (int x = -samples / 2; x <= samples / 2; ++x) {
        for (int y = -samples / 2; y <= samples / 2; ++y) {
            for (int z = -samples / 2; z <= samples / 2; ++z) {
                // 生成偏移方向
                vec3 sampleOffset = vec3(float(x), float(y), float(z)) * offset;
                vec3 sampleDirOffset = normalize(sampleDir + sampleOffset);
                float closestDepth = 0.0;
                // 采样立方体贴图深度值
                if(i == 0){
                    closestDepth = texture(PointShadowMap[0], sampleDirOffset).r * farPlane;
                }else if(i == 1){
                    closestDepth = texture(PointShadowMap[1], sampleDirOffset).r * farPlane;
                }
                else if(i == 2){
                    closestDepth = texture(PointShadowMap[2], sampleDirOffset).r * farPlane;
                }

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