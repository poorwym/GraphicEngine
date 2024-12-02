#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size) // const void* 是通用类型指针，size 是字节数
{
    GLCall(glGenBuffers(1, &m_RendererID)); // 生成缓冲区，并将其 ID 存储在 m_RendererID 中
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // 绑定到 GL_ARRAY_BUFFER 目标
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW)); // 使用 GL_DYNAMIC_DRAW 作为数据使用模式
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID)); // 删除缓冲区
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // 绑定缓冲区
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // 解绑缓冲区
}

void VertexBuffer::setData(const void* data, unsigned int size)
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // 绑定缓冲区

    // 使用 glBufferSubData 更新缓冲区的部分或全部数据
    // 确保新的数据大小不超过缓冲区的原始大小
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));

    // 解绑缓冲区（可选）
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}