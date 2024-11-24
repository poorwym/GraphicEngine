#pragma once
#include "Renderer.h"

struct Triangle {
    glm::vec3 v0;               // 顶点 0 的位置
    glm::vec3 v1;               // 顶点 1 的位置
    glm::vec3 v2;               // 顶点 2 的位置
    glm::vec3 normal;           // 面法线
    glm::vec3 tangent;          // 切线（如果使用法线贴图）
    glm::vec3 bitangent;        // 副切线（如果使用法线贴图）
    glm::vec2 texCoords[3];     // 三个顶点的纹理坐标
    int materialIndex;          // 材质索引
};

class ShaderStorageBuffer
{
private:
    unsigned int m_RendererID;    // 缓冲区对象的 ID
    unsigned int m_Size;          // 缓冲区大小
    unsigned int m_BindingPoint;  // 绑定点索引

public:
    ShaderStorageBuffer(const void* data, unsigned int size, unsigned int bindingPoint);
    ~ShaderStorageBuffer();

    void Bind() const;
    void Unbind() const;

    void SetData(const void* data, unsigned int size, unsigned int offset = 0);

    inline unsigned int GetRendererID() const { return m_RendererID; }
    inline unsigned int GetSize() const { return m_Size; }
    inline unsigned int GetBindingPoint() const { return m_BindingPoint; }
};