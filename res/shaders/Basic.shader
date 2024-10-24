 #shader vertex

 #version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_MVP;

out vec3 FragPos;//传递给片段着色器世界位置
out vec3 Normal;//传递给片段着色器法线向量
out vec2 TexCoords;//传递给片段着色器纹理坐标

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

in vec2 TexCoords;//片段着色器接受的纹理坐标
in vec3 FragPos;//片段着色器接受的世界位置
in vec3 Normal;//片段着色器接受的法线向量

uniform sampler2D diffuseMap;   // 漫反射纹理
uniform sampler2D normalMap;    // 法线贴图
uniform sampler2D specularMap;  // 镜面反射贴图

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;

void main()
{
    vec3 diffuseColor = texture(diffuseMap, TexCoords).rgb;  // 采样漫反射颜色
    vec3 normal = normalize(texture(normalMap, TexCoords).rgb * 2.0 - 1.0); //这里原因略微有些抽象
    vec3 specularColor = texture(specularMap, TexCoords).rgb; // 采样镜面反射颜色

    // 其他光照计算基于这些属性...
    
    FragColor = vec4(diffuseColor, 1.0);  // 最终颜色
}