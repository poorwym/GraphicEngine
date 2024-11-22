#shader vertex
#version 400 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout (location = 3) in float a_Slot0;
layout (location = 4) in float a_Slot1;
layout (location = 5) in float a_Slot2;
layout (location = 6) in float a_Slot3;
layout (location = 7) in float a_Slot4;
layout (location = 8) in float a_Slot5;
layout (location = 9) in float a_Slot6;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    flat int Slots[7];
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
    vs_out.Slots[0] = int(a_Slot0);
    vs_out.Slots[1] = int(a_Slot1);
    vs_out.Slots[2] = int(a_Slot2);
    vs_out.Slots[3] = int(a_Slot3);
    vs_out.Slots[4] = int(a_Slot4);
    vs_out.Slots[5] = int(a_Slot5);
    vs_out.Slots[6] = int(a_Slot6);
}

#shader fragment
#version 400 core
out vec4 FragColor;

in VS_OUT {//������ɫ�����
    vec3 FragPos;//ƬԪλ�ã�����ռ�
    vec2 TexCoords;//��������
    vec3 Normal;//ƬԪ���ߣ�����ռ�
    flat int Slots[7];
} fs_in;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;
uniform sampler2D texture8;
uniform sampler2D texture9;
uniform sampler2D texture10;
uniform sampler2D texture11;
uniform sampler2D texture12;

uniform sampler2D ShadowMap; //slot 31
uniform float farPlane;
uniform vec3 Diffuse;
uniform vec3 Emission;

// ��Դ�ṹ��
struct DirectionalLight
{
    vec3 lightDir;       // ����ⷽ��
    vec3 lightAmbient;   // ������ǿ��
    vec3 lightDiffuse;   // �������ǿ��
    vec3 lightSpecular;  // �����ǿ��
};
uniform DirectionalLight directionalLight;

#define MAX_POINT_LIGHTS 3
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
uniform samplerCube PointShadowMap[3];// slot 27-30

uniform vec3 viewPos;        // �۲���λ��
in vec4 FragPosLightSpace;  // Ƭ����ɫ�����ܵĹ�Դ�ռ�λ��

// ����
const float PI = 3.14159265359;


bool IsVisible(vec3 fragPos);
float ShadowCalculation(vec4 fragPosLightSpace);
vec3 CalculateAmbientColor(vec3 albedo, vec3 ambientLight, float AO);
vec3 CalculateDiffuseColor(vec3 albedo, vec3 lightDiffuse, vec3 lightDir, vec3 normal);
vec3 CalculateSpecularColor(vec3 albedo, vec3 lightSpecular, vec3 lightDir, vec3 normal, vec3 viewDir, float roughness, float metallic);
float PointShadowCalculation(vec3 fragPos, vec3 lightPos, int i);
vec3 GetTextureColor(int slot, vec2 texCoords) {
    if (slot == 0) return texture(texture0, texCoords).rgb;
    else if (slot == 1) return texture(texture1, texCoords).rgb;
    else if (slot == 2) return texture(texture2, texCoords).rgb;
    else if (slot == 3) return texture(texture3, texCoords).rgb;
    else if (slot == 4) return texture(texture4, texCoords).rgb;
    else if (slot == 5) return texture(texture5, texCoords).rgb;
    else if (slot == 6) return texture(texture6, texCoords).rgb;
    else if (slot == 7) return texture(texture7, texCoords).rgb;
    else if (slot == 8) return texture(texture8, texCoords).rgb;
    else if (slot == 9) return texture(texture9, texCoords).rgb;
    else if (slot == 10) return texture(texture10, texCoords).rgb;
    else if (slot == 11) return texture(texture10, texCoords).rgb;
    else if (slot == 12) return texture(texture10, texCoords).rgb;
    else return vec3(1.0, 1.0, 1.0); // Ĭ�Ϸ��ذ�ɫ
}

float GetTextureValue(int slot, vec2 texCoords) {//��ͨ��
    if (slot == 0) return texture(texture0, texCoords).r;
    else if (slot == 1) return texture(texture1, texCoords).r;
    else if (slot == 2) return texture(texture2, texCoords).r;
    else if (slot == 3) return texture(texture3, texCoords).r;
    else if (slot == 4) return texture(texture4, texCoords).r;
    else if (slot == 5) return texture(texture5, texCoords).r;
    else if (slot == 6) return texture(texture6, texCoords).r;
    else if (slot == 7) return texture(texture7, texCoords).r;
    else if (slot == 8) return texture(texture8, texCoords).r;
    else if (slot == 9) return texture(texture9, texCoords).r;
    else if (slot == 10) return texture(texture10, texCoords).r;
    else if (slot == 11) return texture(texture10, texCoords).r;
    else if (slot == 12) return texture(texture10, texCoords).r;
    else return 0.0; // Ĭ�Ϸ���0.0
}

// ������
void main()
{
    vec3 finalColor = vec3(0.0);
    vec3 ambient = fs_in.Slots[0] != -1 ? GetTextureColor(fs_in.Slots[0], fs_in.TexCoords) : Diffuse;// ��������ɫ 0
    vec3 diffuse = ambient;
    vec3 specular = ambient;
    vec3 normal = fs_in.Slots[1] != -1 ? GetTextureColor(fs_in.Slots[1], fs_in.TexCoords) : fs_in.Normal; // ���� 1
    float metallic = fs_in.Slots[2] != -1 ? GetTextureValue(fs_in.Slots[2], fs_in.TexCoords) : 0.0; // ������ 2
    float roughness = fs_in.Slots[3] != -1 ? GetTextureValue(fs_in.Slots[3], fs_in.TexCoords) : 0.5; // �ֲڶ� 3
    float AO = fs_in.Slots[4] != -1 ? GetTextureValue(fs_in.Slots[4], fs_in.TexCoords) : 1.0; // �������ڱ� 4
    vec3 emission = fs_in.Slots[5] != -1 ? GetTextureColor(fs_in.Slots[5], fs_in.TexCoords) : Emission; // �Է��� 5
    float alpha = fs_in.Slots[6] != -1 ? GetTextureValue(fs_in.Slots[6], fs_in.TexCoords) : 1.0;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);    if(metallic > 0.0) {
        specular = diffuse; // ���ڽ������ʣ�ʹ��Albedo��ɫ��ΪF0
        diffuse = vec3(0.0); // ����û��������
    }

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
float ShadowCalculation(vec4 fragPosLightSpace) //PCF
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

float PointShadowCalculation(vec3 fragPos, vec3 lightPos, int i) { //PCF
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
                float closestDepth = 0.0;
                // ������������ͼ���ֵ
                if(i == 0){
                    closestDepth = texture(PointShadowMap[0], sampleDirOffset).r * farPlane;
                }else if(i == 1){
                    closestDepth = texture(PointShadowMap[1], sampleDirOffset).r * farPlane;
                }
                else if(i == 2){
                    closestDepth = texture(PointShadowMap[2], sampleDirOffset).r * farPlane;
                }

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