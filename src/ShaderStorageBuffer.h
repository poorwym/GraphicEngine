#pragma once
#include "Renderer.h"


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