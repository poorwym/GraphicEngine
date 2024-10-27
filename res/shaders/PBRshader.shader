#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_MVP;
uniform mat4 u_Model;    

out vec3 FragPos; // ���ݸ�Ƭ����ɫ������λ��
out vec3 Normal;  // ���ݸ�Ƭ����ɫ����������
out vec2 TexCoords; // ���ݸ�Ƭ����ɫ����������

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1.0);
    FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // �޸�
    Normal = mat3(transpose(inverse(u_Model))) * a_Normal; // �޸�
    TexCoords = a_TexCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Ƭ����ɫ�����ܵ���������
in vec3 FragPos;   // Ƭ����ɫ�����ܵ�����λ��
in vec3 Normal;    // Ƭ����ɫ�����ܵķ�������

// ��������
uniform bool hasNormalMap;
uniform bool hasEmissionMap;
uniform bool hasHeightMap;
uniform bool hasAlbedoMap;
uniform bool hasAO;
uniform bool hasMetallicMap;
uniform bool hasRoughnessMap;

// ���������
uniform sampler2D AlbedoMap;       // ��������ͼ
uniform sampler2D NormalMap;       // ������ͼ
uniform sampler2D MetallicMap;     // ��������ͼ
uniform sampler2D RoughnessMap;    // �ֲڶ���ͼ
uniform sampler2D AOMap;           // �����ڱ���ͼ
uniform sampler2D EmissionMap;     // �Է�����ͼ
uniform sampler2D HeightMap;       // �߶���ͼ����ѡ��

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

// ����
const float PI = 3.14159265359;

// ��������
vec3 getNormalFromMap();
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

// ������
void main()
{
    // 1. ������������
    vec3 albedo = hasAlbedoMap ? texture(AlbedoMap, TexCoords).rgb : vec3(1.0);
    albedo = pow(albedo, vec3(2.2)); // GammaУ��

    float metallic = hasMetallicMap ? texture(MetallicMap, TexCoords).r : 0.0;
    float roughness = hasRoughnessMap ? texture(RoughnessMap, TexCoords).r : 0.5;
    float ao = hasAO ? texture(AOMap, TexCoords).r : 1.0;

    // 2. ��ȡ����
    vec3 N = normalize(Normal);
    if (hasNormalMap)
    {
        N = getNormalFromMap();
    }

    // 3. ��������
    vec3 V = normalize(viewPos - FragPos);

    // 4. ���������ʣ�F0��
    vec3 F0 = vec3(0.04); // �ǽ������ʵ�F0
    F0 = mix(F0, albedo, metallic); // �������ʵ�F0ΪAlbedo��ɫ

    // 5. �����⹱�׳�ʼ��
    vec3 ambient = directionalLight.lightAmbient * albedo * ao;

    // 6. ����⹱��
    // �����������
    vec3 L = normalize(-directionalLight.lightDir); // ���߷���
    vec3 H = normalize(V + L); // ������

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // ��ֹ����0
    vec3 specular = numerator / denominator;

    // ���������
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic; // ��������û��������

    vec3 diffuse = kD * albedo / PI;

    // �������ܹ���
    float NdotL = max(dot(N, L), 0.0);
    vec3 directionalLightContribution = (diffuse + specular) * directionalLight.lightDiffuse * NdotL;

    // 7. ���Դ����
    for(int i = 0; i < numPointLights; i++)
    {
        // ��Դ����
        vec3 Lp = normalize(pointLights[i].lightPos - FragPos);
        vec3 Hp = normalize(V + Lp);

        // ������
        float NdotLp = max(dot(N, Lp), 0.0);
        float NdotHp = max(dot(N, Hp), 0.0);
        float VdotHp = max(dot(V, Hp), 0.0);

        // �ֲ�����
        float NDF_p = DistributionGGX(N, Hp, roughness);
        // �����ڱ�
        float G_p = GeometrySmith(N, V, Lp, roughness);
        // Fresnel
        vec3 F_p = fresnelSchlick(NdotHp, F0);

        // Cook-Torrance BRDF
        vec3 numerator_p = NDF_p * G_p * F_p;
        float denominator_p = 4.0 * max(dot(N, V), 0.0) * max(dot(N, Lp), 0.0) + 0.001;
        vec3 specular_p = numerator_p / denominator_p;

        // ������
        vec3 kS_p = F_p;
        vec3 kD_p = vec3(1.0) - kS_p;
        kD_p *= 1.0 - metallic;

        vec3 diffuse_p = kD_p * albedo / PI;

        // ���Դ��˥��
        float distance = length(pointLights[i].lightPos - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + 
                                   pointLights[i].quadratic * (distance * distance));

        // ���Դ�Ĺ���
        vec3 pointLightContribution = (diffuse_p + specular_p) * pointLights[i].lightDiffuse * NdotLp * attenuation;

        ambient += pointLights[i].lightAmbient * albedo * ao;
        directionalLightContribution += pointLightContribution;
    }

    // 8. �Է���
    vec3 emission = hasEmissionMap ? texture(EmissionMap, TexCoords).rgb : vec3(0.0);

    // 9. �ϲ����й���
    vec3 color = ambient + directionalLightContribution + emission;

    // Gamma У��
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);  // ������ɫ
}

// ��ȡ������ͼ�еķ���
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(NormalMap, TexCoords).rgb;
    tangentNormal = tangentNormal * 2.0 - 1.0;
    return normalize(tangentNormal);
}

// �ֲ�������GGX/Trowbridge-Reitz
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
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}