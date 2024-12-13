#version 450 core

struct Material {
    float AlbedoMapIndex; //0
    float NormalMapIndex; //1
    float MetallicMapIndex; //2
    float RoughnessMapIndex; //3
    float AOMapIndex; //4
    float EmissionMapIndex; //5
    float AlphaMapIndex; //6
    float HeightMapIndex; //7
    //Ka
    vec4 Ambient;
    //Kd
    vec4 Diffuse;
    //Ks
    vec4 Specular;
    //Ke
    vec4 Emission;
    //d
    float Dissolve;
    //illum
    float Illum;
    //Ni
    float OpticalDensity;
    //Ns
    float SpecularExponent;
    float BumpMutiplier;
    float padding2;
    float padding3;
    float padding4;
};

struct Vertex {
    vec4 Position;
    vec4 Normal;
    vec4 TexCoords;
    vec4 Tangent;
    vec4 Bitangent;
    Material material;
};

// 定义工作组大小
layout(local_size_x = 512) in;

// Shader Storage Buffer Object (SSBO) 绑定到位置 0
layout(std430, binding = 0) buffer InputBuffer {
    Vertex inputVertices[];
};

layout(std430, binding = 1) buffer OutputBuffer {
    Vertex outputVertices[];
};

// Uniform 变量：模型矩阵和法线矩阵
uniform mat4 u_Model;
uniform mat3 normalMatrix;

// Uniform 变量：顶点数量
uniform int numVertices;

void main(){
    uint gid = gl_GlobalInvocationID.x;
    // 处理所有顶点
    for(uint i = gid; i < numVertices; i += gl_WorkGroupSize.x){
        // 位置变换
        outputVertices[i].Position = u_Model * inputVertices[i].Position;
        // 法线、切线、副切线变换
        outputVertices[i].Normal = vec4(normalize(normalMatrix * inputVertices[i].Normal.xyz), 0.0);
        outputVertices[i].Tangent = vec4(normalize(normalMatrix * inputVertices[i].Tangent.xyz), 0.0);
        outputVertices[i].Bitangent = vec4(normalize(normalMatrix * inputVertices[i].Bitangent.xyz), 0.0);

        outputVertices[i].TexCoords =  inputVertices[i].TexCoords;
        outputVertices[i].material =   inputVertices[i].material;
    }
}