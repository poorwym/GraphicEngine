 #shader vertex

 #version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_MVP;

out vec3 FragPos;//���ݸ�Ƭ����ɫ������λ��
out vec3 Normal;//���ݸ�Ƭ����ɫ����������
out vec2 TexCoords;//���ݸ�Ƭ����ɫ����������

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1.0);
    FragPos = vec3(gl_Position);
    Normal = a_Normal;
    TexCoords = a_TexCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;//Ƭ����ɫ�����ܵ���������
in vec3 FragPos;//Ƭ����ɫ�����ܵ�����λ��
in vec3 Normal;//Ƭ����ɫ�����ܵķ�������

uniform sampler2D diffuseMap;   // ����������
uniform sampler2D normalMap;    // ������ͼ
uniform sampler2D specularMap;  // ���淴����ͼ

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;

void main()
{
    vec3 diffuseColor = texture(diffuseMap, TexCoords).rgb;  // ������������ɫ
    vec3 normal = normalize(texture(normalMap, TexCoords).rgb * 2.0 - 1.0); //����ԭ����΢��Щ����
    vec3 specularColor = texture(specularMap, TexCoords).rgb; // �������淴����ɫ

    // �������ռ��������Щ����...
    
    FragColor = vec4(diffuseColor, 1.0);  // ������ɫ
}