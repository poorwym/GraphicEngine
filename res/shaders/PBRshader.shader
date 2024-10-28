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
    vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // ����ռ�λ��
    vs_out.Normal = mat3(transpose(inverse(u_Model))) * a_Normal; //����ռ䷨������
    FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.TexCoords = a_TexCoords;
    vs_out.Tangent = a_Tangent;
    vs_out.Bitangent = a_Bitangent;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT {//������ɫ�����
    vec3 FragPos;//ƬԪλ�ã�����ռ�
    vec2 TexCoords;//��������
    vec3 Tangent;//����
    vec3 Bitangent;//������
    vec3 Normal;//ƬԪ���ߣ�����ռ�
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



float ShadowCalculation(vec4 fragPosLightSpace);
// ������
void main()
{
    vec3 AmbientColor = texture(AlbedoMap, fs_in.TexCoords).rgb;
    float dirShadow = ShadowCalculation(FragPosLightSpace);
    FragColor = vec4(AmbientColor * (1 - dirShadow) ,1.0);
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

const int sampleRate = 5;
const float bias = 0.0005;
float ShadowCalculation(vec4 fragPosLightSpace)
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