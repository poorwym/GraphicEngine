// ShaderStorageBuffer.h

#pragma once
#include <GL/glew.h>
#include <cstddef>
#include <stdexcept>
#include <string>

class ShaderStorageBuffer
{
private:
    GLuint m_RendererID;
    GLsizeiptr m_Size;
    GLuint m_BindingPoint;

public:
    ShaderStorageBuffer();
    // 构造函数：创建缓冲区并上传数据
    ShaderStorageBuffer(const void* data, unsigned int size, unsigned int bindingPoint);

    // 析构函数：删除缓冲区
    ~ShaderStorageBuffer();

    // 绑定缓冲区
    void Bind() const;

    // 解绑缓冲区
    void Unbind() const;

    // 更新缓冲区数据（部分更新）
    void SetData(const void* data, unsigned int size, unsigned int offset = 0) const;

    // 映射缓冲区到CPU内存
    void* Map(GLenum access = GL_READ_ONLY) const;

    // 解除映射缓冲区
    void Unmap() const;

    // 获取缓冲区ID
    GLuint GetID() const { return m_RendererID; }

    // 获取缓冲区大小
    GLsizeiptr GetSize() const { return m_Size; }

    // 获取绑定点
    GLuint GetBindingPoint() const { return m_BindingPoint; }

    // 禁止拷贝构造和拷贝赋值
    ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;
    ShaderStorageBuffer& operator=(const ShaderStorageBuffer&) = delete;

    // 允许移动构造和移动赋值
    ShaderStorageBuffer(ShaderStorageBuffer&& other) noexcept;
    ShaderStorageBuffer& operator=(ShaderStorageBuffer&& other) noexcept;
};