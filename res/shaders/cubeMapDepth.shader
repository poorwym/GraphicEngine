#shader vertex
#version 330 core
layout (location = 0) in vec3 a_Position;

uniform mat4 ShadowMatrices[6]; // 六个方向的视图投影矩阵
uniform mat4 u_Model;
uniform vec3 lightPos;          // 点光源位置
uniform float farPlane;         // 远平面
out vec3 FragPos;               // 世界空间位置

void main()
{
    FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    gl_Position = ShadowMatrices[gl_InstanceID] * vec4(FragPos - lightPos, 1.0); // 使用 gl_InstanceID
}

#shader fragment
#version 330 core

in vec3 FragPos;
uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    // 将深度值线性化
    float lightDistance = length(FragPos - lightPos);
    gl_FragDepth = lightDistance / farPlane; // 深度值范围 [0,1]
}