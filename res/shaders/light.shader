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

uniform sampler2D diffuseMap;   // ����������
uniform sampler2D normalMap;    // ������ͼ
uniform sampler2D specularMap;  // ���淴����ͼ

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;

// �����������uniform
uniform vec3 lightDir;       // ����ⷽ��
uniform vec3 lightAmbient;   // ������ǿ��
uniform vec3 lightDiffuse;   // �������ǿ��
uniform vec3 lightSpecular;  // �����ǿ��

uniform vec3 viewPos;        // �۲���λ��

void main()
{
    // ��������
    vec3 diffuseColor = texture(diffuseMap, TexCoords).rgb * albedo;
    vec3 normalMapColor = texture(normalMap, TexCoords).rgb;
    vec3 normal = normalize(normalMapColor * 2.0 - 1.0); // ����ӳ��
    vec3 specularColor = texture(specularMap, TexCoords).rgb; // �������淴����ɫ

    // ������
    vec3 ambient = lightAmbient * diffuseColor;

    // �������
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, normalize(-lightDir)), 0.0);
    vec3 diffuse = lightDiffuse * diff * diffuseColor;

    // �����
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);
    
    // ʹ�� roughness ���� specular exponent
    float specularExponent = mix(32.0, 2.0, roughness); // 32 Ϊ�߹�ָ�����ֲڶ�Խ�ߣ�ָ��Խ��
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
    
    vec3 specular = lightSpecular * spec * specularColor;

    // �ϲ����й���
    vec3 result = ambient + diffuse + specular;
    
    // ���ǽ����Ⱥʹֲڶȣ��򵥴����ɸ���PBR�����һ��������
    result = mix(diffuseColor, result, metallic);

    FragColor = vec4(result, 1.0);  // ������ɫ
}

