#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_MVP;
uniform mat4 u_Model;    

out vec3 FragPos; // 传递给片段着色器世界位置
out vec3 Normal;  // 传递给片段着色器法线向量
out vec2 TexCoords; // 传递给片段着色器纹理坐标

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1.0);
    FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // 修改
    Normal = mat3(transpose(inverse(u_Model))) * a_Normal; // 修改
    TexCoords = a_TexCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // 片段着色器接受的纹理坐标
in vec3 FragPos;   // 片段着色器接受的世界位置
in vec3 Normal;    // 片段着色器接受的法线向量

// 材质属性
uniform bool hasNormalMap;
uniform bool hasEmissionMap;
uniform bool hasHeightMap;
uniform bool hasAlbedoMap;
uniform bool hasAO;
uniform bool hasMetallicMap;
uniform bool hasRoughnessMap;

// 纹理采样器
uniform sampler2D AlbedoMap;       // 漫反射贴图
uniform sampler2D NormalMap;       // 法线贴图
uniform sampler2D MetallicMap;     // 金属度贴图
uniform sampler2D RoughnessMap;    // 粗糙度贴图
uniform sampler2D AOMap;           // 环境遮蔽贴图
uniform sampler2D EmissionMap;     // 自发光贴图
uniform sampler2D HeightMap;       // 高度贴图（可选）

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

// 常量
const float PI = 3.14159265359;

// 函数声明
vec3 getNormalFromMap();
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

// 主函数
void main()
{
    // 1. 采样材质属性
    vec3 albedo = hasAlbedoMap ? texture(AlbedoMap, TexCoords).rgb : vec3(1.0);
    albedo = pow(albedo, vec3(2.2)); // Gamma校正

    float metallic = hasMetallicMap ? texture(MetallicMap, TexCoords).r : 0.0;
    float roughness = hasRoughnessMap ? texture(RoughnessMap, TexCoords).r : 0.5;
    float ao = hasAO ? texture(AOMap, TexCoords).r : 1.0;

    // 2. 获取法线
    vec3 N = normalize(Normal);
    if (hasNormalMap)
    {
        N = getNormalFromMap();
    }

    // 3. 视线向量
    vec3 V = normalize(viewPos - FragPos);

    // 4. 基础反射率（F0）
    vec3 F0 = vec3(0.04); // 非金属材质的F0
    F0 = mix(F0, albedo, metallic); // 金属材质的F0为Albedo颜色

    // 5. 环境光贡献初始化
    vec3 ambient = directionalLight.lightAmbient * albedo * ao;

    // 6. 方向光贡献
    // 方向光漫反射
    vec3 L = normalize(-directionalLight.lightDir); // 光线方向
    vec3 H = normalize(V + L); // 半向量

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 防止除以0
    vec3 specular = numerator / denominator;

    // 漫反射计算
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic; // 金属材质没有漫反射

    vec3 diffuse = kD * albedo / PI;

    // 方向光的总贡献
    float NdotL = max(dot(N, L), 0.0);
    vec3 directionalLightContribution = (diffuse + specular) * directionalLight.lightDiffuse * NdotL;

    // 7. 点光源贡献
    for(int i = 0; i < numPointLights; i++)
    {
        // 光源方向
        vec3 Lp = normalize(pointLights[i].lightPos - FragPos);
        vec3 Hp = normalize(V + Lp);

        // 漫反射
        float NdotLp = max(dot(N, Lp), 0.0);
        float NdotHp = max(dot(N, Hp), 0.0);
        float VdotHp = max(dot(V, Hp), 0.0);

        // 分布函数
        float NDF_p = DistributionGGX(N, Hp, roughness);
        // 几何遮蔽
        float G_p = GeometrySmith(N, V, Lp, roughness);
        // Fresnel
        vec3 F_p = fresnelSchlick(NdotHp, F0);

        // Cook-Torrance BRDF
        vec3 numerator_p = NDF_p * G_p * F_p;
        float denominator_p = 4.0 * max(dot(N, V), 0.0) * max(dot(N, Lp), 0.0) + 0.001;
        vec3 specular_p = numerator_p / denominator_p;

        // 漫反射
        vec3 kS_p = F_p;
        vec3 kD_p = vec3(1.0) - kS_p;
        kD_p *= 1.0 - metallic;

        vec3 diffuse_p = kD_p * albedo / PI;

        // 点光源的衰减
        float distance = length(pointLights[i].lightPos - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + 
                                   pointLights[i].quadratic * (distance * distance));

        // 点光源的贡献
        vec3 pointLightContribution = (diffuse_p + specular_p) * pointLights[i].lightDiffuse * NdotLp * attenuation;

        ambient += pointLights[i].lightAmbient * albedo * ao;
        directionalLightContribution += pointLightContribution;
    }

    // 8. 自发光
    vec3 emission = hasEmissionMap ? texture(EmissionMap, TexCoords).rgb : vec3(0.0);

    // 9. 合并所有光照
    vec3 color = ambient + directionalLightContribution + emission;

    // Gamma 校正
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);  // 最终颜色
}

// 获取法线贴图中的法线
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(NormalMap, TexCoords).rgb;
    tangentNormal = tangentNormal * 2.0 - 1.0;
    return normalize(tangentNormal);
}

// 分布函数：GGX/Trowbridge-Reitz
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
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