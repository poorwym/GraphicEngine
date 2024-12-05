// ShaderStorageBuffer.cpp

#include "ShaderStorageBuffer.h"
#include <iostream>

// 假设您有一个 Renderer.h，其中定义了 GLCall 宏
#include "Renderer.h"

ShaderStorageBuffer::ShaderStorageBuffer()
{
}

// 构造函数：创建缓冲区并上传数据
ShaderStorageBuffer::ShaderStorageBuffer(const void* data, unsigned int size, unsigned int bindingPoint)
    : m_Size(size), m_BindingPoint(bindingPoint)
{
    GLCall(glGenBuffers(1, &m_RendererID)); // 生成缓冲区对象
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID)); // 绑定缓冲区到 SSBO 目标
    GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW)); // 分配内存并初始化数据
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_RendererID)); // 将缓冲区绑定到指定的绑定点
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // 解除绑定
}

// 移动构造函数
ShaderStorageBuffer::ShaderStorageBuffer(ShaderStorageBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID), m_Size(other.m_Size), m_BindingPoint(other.m_BindingPoint)
{
    other.m_RendererID = 0;
    other.m_Size = 0;
    other.m_BindingPoint = 0;
}

// 移动赋值运算符
ShaderStorageBuffer& ShaderStorageBuffer::operator=(ShaderStorageBuffer&& other) noexcept
{
    if (this != &other)
    {
        // 先删除当前的缓冲区
        GLCall(glDeleteBuffers(1, &m_RendererID));

        // 移动数据
        m_RendererID = other.m_RendererID;
        m_Size = other.m_Size;
        m_BindingPoint = other.m_BindingPoint;

        // 重置其他对象
        other.m_RendererID = 0;
        other.m_Size = 0;
        other.m_BindingPoint = 0;
    }
    return *this;
}

// 析构函数：删除缓冲区
ShaderStorageBuffer::~ShaderStorageBuffer()
{
    if (m_RendererID != 0)
    {
        GLCall(glDeleteBuffers(1, &m_RendererID)); // 删除缓冲区对象
    }
}

// 绑定缓冲区
void ShaderStorageBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID)); // 绑定缓冲区
}

// 解绑缓冲区
void ShaderStorageBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // 解除绑定
}

// 更新缓冲区数据（部分更新）
void ShaderStorageBuffer::SetData(const void* data, unsigned int size, unsigned int offset) const
{
    Bind();
    GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data)); // 更新缓冲区的数据
    Unbind();
}

// 映射缓冲区到CPU内存
void* ShaderStorageBuffer::Map(GLenum access) const
{
    Bind();
    void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, access);
    Unbind();

    if (!ptr)
    {
        std::cerr << "Failed to map Shader Storage Buffer." << std::endl;
        throw std::runtime_error("ShaderStorageBuffer::Map - glMapBuffer failed.");
    }

    return ptr;
}

// 解除映射缓冲区
void ShaderStorageBuffer::Unmap() const
{
    Bind();
    GLboolean result = glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    Unbind();

    if (result == GL_FALSE)
    {
        std::cerr << "Failed to unmap Shader Storage Buffer. Data may be corrupted." << std::endl;
        throw std::runtime_error("ShaderStorageBuffer::Unmap - glUnmapBuffer failed.");
    }
}