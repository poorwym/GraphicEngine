#include "ShaderStorageBuffer.h"
#include <iostream>

#include "Renderer.h"

ShaderStorageBuffer::ShaderStorageBuffer(const void* data, unsigned int size, unsigned int bindingPoint)
    : m_Size(size), m_BindingPoint(bindingPoint)
{
    GLCall(glGenBuffers(1, &m_RendererID)); // 生成缓冲区对象
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID)); // 绑定缓冲区到 SSBO 目标
    GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW)); // 分配内存并初始化数据
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_RendererID)); // 将缓冲区绑定到指定的绑定点
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // 解除绑定
}

ShaderStorageBuffer::~ShaderStorageBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID)); // 删除缓冲区对象
}

void ShaderStorageBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID)); // 绑定缓冲区
}

void ShaderStorageBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // 解除绑定
}

void ShaderStorageBuffer::SetData(const void* data, unsigned int size, unsigned int offset)
{
    Bind();
    GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data)); // 更新缓冲区的数据
    Unbind();
}