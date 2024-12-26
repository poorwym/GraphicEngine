#version 450 core
struct Vertex {
    vec4 Position;
    vec4 Normal;
    vec4 TexCoords;
    vec4 Tangent;
    vec4 Bitangent;
    float MaterialIndex;
    float padding1;
    float padding2;
    float padding3;
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
        // 纹理坐标变换
        outputVertices[i].TexCoords =  inputVertices[i].TexCoords;
        // 材质索引变换
        outputVertices[i].MaterialIndex = inputVertices[i].MaterialIndex;
    }
}