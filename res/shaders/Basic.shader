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

uniform vec3 Diffuse;

uniform bool hasAlbedoMap;


// ���������
uniform sampler2D AlbedoMap;       // ��������ͼ         slot 0
uniform sampler2D ViewDepthMap; //�Ӿ������ͼ slot 10


uniform vec3 viewPos;        // �۲���λ��
in vec4 FragPosLightSpace;  // Ƭ����ɫ�����ܵĹ�Դ�ռ�λ��

// ����
const float PI = 3.14159265359;


bool IsVisible(vec3 fragPos);

// ������
void main()
{
    vec3 ambient = hasAlbedoMap ? texture(AlbedoMap, fs_in.TexCoords).rgb : Diffuse;// ��������ɫ
    if(IsVisible(fs_in.FragPos)) FragColor = vec4(vec3(ambient),1.0);
}

bool IsVisible(vec3 fragPos){
    vec3 projCoords = fragPos;
    // ��ȡ��ǰƬ�ε����ֵ
    float closestDepth = texture(ViewDepthMap, projCoords.xy).r;
    // ��ȡ��ǰƬ���ڹ�ռ�����ֵ
    float currentDepth = projCoords.z;
    return closestDepth == currentDepth;
}