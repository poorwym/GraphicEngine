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

// ���幤�����С
layout(local_size_x = 512) in;

// Shader Storage Buffer Object (SSBO) �󶨵�λ�� 0
layout(std430, binding = 0) buffer InputBuffer {
    Vertex inputVertices[];
};

layout(std430, binding = 1) buffer OutputBuffer {
    Vertex outputVertices[];
};

// Uniform ������ģ�;���ͷ��߾���
uniform mat4 u_Model;
uniform mat3 normalMatrix;

// Uniform ��������������
uniform int numVertices;

void main(){
    uint gid = gl_GlobalInvocationID.x;
    // �������ж���
    for(uint i = gid; i < numVertices; i += gl_WorkGroupSize.x){
        // λ�ñ任
        outputVertices[i].Position = u_Model * inputVertices[i].Position;
        // ���ߡ����ߡ������߱任
        outputVertices[i].Normal = vec4(normalize(normalMatrix * inputVertices[i].Normal.xyz), 0.0);
        outputVertices[i].Tangent = vec4(normalize(normalMatrix * inputVertices[i].Tangent.xyz), 0.0);
        outputVertices[i].Bitangent = vec4(normalize(normalMatrix * inputVertices[i].Bitangent.xyz), 0.0);
        // ��������任
        outputVertices[i].TexCoords =  inputVertices[i].TexCoords;
        // ���������任
        outputVertices[i].MaterialIndex = inputVertices[i].MaterialIndex;
    }
}