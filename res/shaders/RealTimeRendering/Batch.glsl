#shader vertex
#version 450 core
#include "layout.glsl"
uniform mat4 u_MVP;
uniform mat4 u_Model;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    flat int MaterialIndex;
    flat int pad[7];
    mat3 TBN;
} vs_out;

void main()
{
    gl_Position = u_MVP * a_Position;
    vs_out.FragPos = a_Position.xyz; // 世界空间位置
    vs_out.Normal = a_Normal.xyz; //世界空间法线向量
    vs_out.TexCoords = a_TexCoords.xy;
    vs_out.Tangent = a_Tangent.xyz;
    vs_out.Bitangent = a_Bitangent.xyz;
    //vs_out.MaterialIndex = int(1);
    vs_out.MaterialIndex = int(a_MaterialIndex);

    vec3 T = normalize(vec3(a_Tangent));
    vec3 B = normalize(vec3(a_Bitangent));
    vec3 N = normalize(vec3(a_Normal));

    // 构建 TBN 矩阵
    vs_out.TBN = mat3(T, B, N);
}

#shader fragment
#version 450 core
out vec4 FragColor;
#include "Macro.glsl"
#include "struct.glsl"
#include "uniform.glsl"
#include "Head.glsl"

layout(std430, binding = 0) buffer MaterialBuffer {
    Material materials[];
};


#include "PBR.glsl"
#include "material.glsl"

in VS_OUT {//顶点着色器输出
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    flat int MaterialIndex;
    flat int pad[7];
    mat3 TBN;
} fs_in;

void main()
{
    vec3 finalColor = vec3(0.0);
    //计算颜色
    vec3 ambient = GetAmbient(fs_in.MaterialIndex, fs_in.TexCoords);
    vec3 diffuse = GetDiffuse(fs_in.MaterialIndex, fs_in.TexCoords);
    vec3 specular = GetSpecular(fs_in.MaterialIndex, fs_in.TexCoords);
    //计算法线
    
    vec3 normal = vec3(0.0);
    if( materials[fs_in.MaterialIndex].NormalMapIndex != -1){
        vec3 sampledNormal =  GetNormal(fs_in.MaterialIndex, fs_in.TexCoords); 
        sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
        normal = normalize(fs_in.TBN * sampledNormal);
    }
    else{
        normal = normalize(fs_in.Normal);
    }
    //计算其他不是很重要的参数
    float metallic = GetMetallic(fs_in.MaterialIndex, fs_in.TexCoords);
    float roughness = GetRoughness(fs_in.MaterialIndex, fs_in.TexCoords);
    float AO = GetAO(fs_in.MaterialIndex, fs_in.TexCoords);
    vec3 emission = GetEmission(fs_in.MaterialIndex, fs_in.TexCoords);
    float alpha = GetAlpha(fs_in.MaterialIndex, fs_in.TexCoords);

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
    for(int i = 0; i < numDirectionalLights; i++)
    {
        vec3 ambientColor = CalculateAmbientColor(ambient, directionalLights[i].lightAmbient, AO);
        vec3 diffuseColor = CalculateDiffuseColor(diffuse, directionalLights[i].lightDiffuse, directionalLights[i].lightDir, normal);
        vec3 specularColor = CalculateSpecularColor(specular, directionalLights[i].lightSpecular, directionalLights[i].lightDir, normal, viewDir, roughness, metallic);
        finalColor += ambientColor + diffuseColor + specularColor + emission;
    }
    
    for(int i = 0; i < numPointLights; i++)
    {
        vec3 lightDir = pointLights[i].lightPos - fs_in.FragPos;
        vec3 pointAmbient = CalculateAmbientColor(ambient, pointLights[i].lightAmbient, AO);
        vec3 pointDiffuse = CalculateDiffuseColor(diffuse, pointLights[i].lightDiffuse, lightDir, normal);
        vec3 pointSpecular = CalculateSpecularColor(specular, pointLights[i].lightSpecular, lightDir, normal, viewDir, roughness, metallic);
        float distance = distance(fs_in.FragPos, pointLights[i].lightPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
        pointAmbient = pointAmbient * attenuation;
        pointDiffuse = pointDiffuse * attenuation;
        pointSpecular = pointSpecular * attenuation;
        finalColor += pointAmbient + pointDiffuse + pointSpecular;
    }
    FragColor = vec4(finalColor, 1.0);
    if( abs(gl_FragCoord.z - focusDepth) <= 0.0){
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
}
