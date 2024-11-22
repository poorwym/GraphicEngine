#shader vertex
#version 400 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in float a_Slots[7];

uniform mat4 ShadowMatrices[6]; // �����������ͼͶӰ����
uniform mat4 u_Model;
uniform vec3 lightPos;          // ���Դλ��
uniform float farPlane;         // Զƽ��
out vec3 FragPos;               // ����ռ�λ��

void main()
{
    FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    gl_Position = ShadowMatrices[gl_InstanceID] * vec4(FragPos - lightPos, 1.0); // ʹ�� gl_InstanceID
}

#shader fragment
#version 400 core

in vec3 FragPos;
uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    // �����ֵ���Ի�
    float lightDistance = length(FragPos - lightPos);
    gl_FragDepth = lightDistance / farPlane; // ���ֵ��Χ [0,1]
}