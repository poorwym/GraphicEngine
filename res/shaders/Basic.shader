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
    vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // 世界空间位置
    vs_out.Normal = mat3(transpose(inverse(u_Model))) * a_Normal; //世界空间法线向量
    FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.TexCoords = a_TexCoords;
    vs_out.Tangent = a_Tangent;
    vs_out.Bitangent = a_Bitangent;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT {//顶点着色器输出
    vec3 FragPos;//片元位置，世界空间
    vec2 TexCoords;//纹理坐标
    vec3 Tangent;//切线
    vec3 Bitangent;//副切线
    vec3 Normal;//片元法线，世界空间
} fs_in;

uniform vec3 Diffuse;

uniform bool hasAlbedoMap;


// 纹理采样器
uniform sampler2D AlbedoMap;       // 漫反射贴图         slot 0
uniform sampler2D ViewDepthMap; //视觉深度贴图 slot 10


uniform vec3 viewPos;        // 观察者位置
in vec4 FragPosLightSpace;  // 片段着色器接受的光源空间位置

// 常量
const float PI = 3.14159265359;


bool IsVisible(vec3 fragPos);

// 主函数
void main()
{
    vec3 ambient = hasAlbedoMap ? texture(AlbedoMap, fs_in.TexCoords).rgb : Diffuse;// 本来的颜色
    if(IsVisible(fs_in.FragPos)) FragColor = vec4(vec3(ambient),1.0);
}

bool IsVisible(vec3 fragPos){
    vec3 projCoords = fragPos;
    // 获取当前片段的深度值
    float closestDepth = texture(ViewDepthMap, projCoords.xy).r;
    // 获取当前片段在光空间的深度值
    float currentDepth = projCoords.z;
    return closestDepth == currentDepth;
}