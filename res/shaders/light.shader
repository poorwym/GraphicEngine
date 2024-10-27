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
    FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // ����λ��
    Normal = mat3(transpose(inverse(u_Model))) * a_Normal; // ���編��
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

// �����ṹ��
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
    float constant;   //����˥��
    float linear;     //����˥��
    float quadratic;  //���η�˥��
};

uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform vec3 viewPos;        // �۲���λ��
uniform int numPointLights;

void main()
{
    // ��������
    vec3 diffuseColor = texture(diffuseMap, TexCoords).rgb * albedo;
    vec3 normalMapColor = texture(normalMap, TexCoords).rgb;
    vec3 normal = normalize(normalMapColor * 2.0 - 1.0); // ����ӳ��
    vec3 specularColor = texture(specularMap, TexCoords).rgb; // �������淴����ɫ

    // ������
    vec3 ambient = directionalLight.lightAmbient * diffuseColor;

    // �������
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, normalize(-directionalLight.lightDir)), 0.0);
    vec3 diffuse = directionalLight.lightDiffuse * diff * diffuseColor;

    // �����
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(directionalLight.lightDir, norm);
    
    // ʹ�� roughness ���� specular exponent
    float specularExponent = mix(16.0, 2.0, roughness); // 16 Ϊ�߹�ָ�����ֲڶ�Խ�ߣ�ָ��Խ��
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
    
    vec3 specular = directionalLight.lightSpecular * spec * specularColor;

    //���е��Դ�Ĺ���
    for(int i = 0; i < numPointLights; i++)
    {
        // ��Դ����;���
        vec3 lightDir = normalize(pointLights[i].lightPos - FragPos);
        float distance = length(pointLights[i].lightPos - FragPos);

        // ˥��
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + 
                                   pointLights[i].quadratic * (distance * distance));

        // ������
        vec3 ambientComponent = pointLights[i].lightAmbient * diffuseColor;

        // �������
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuseComponent = pointLights[i].lightDiffuse * diff * diffuseColor;

        // �����
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
        vec3 specularComponent = pointLights[i].lightSpecular * spec * specularColor;

        // Ӧ��˥��
        ambientComponent *= attenuation;
        diffuseComponent *= attenuation;
        specularComponent *= attenuation;

        // �ۼӵ��Դ�Ĺ���
        ambient += ambientComponent;
        diffuse += diffuseComponent;
        specular += specularComponent;
    }
    
    // �ϲ����й���
    vec3 result = ambient + diffuse + specular;
    
    // ���ǽ����Ⱥʹֲڶȣ��򵥴����ɸ���PBR�����һ��������
    result = mix(diffuseColor, result, metallic);

    FragColor = vec4(result, 1.0);  // ������ɫ
}

