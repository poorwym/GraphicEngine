#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)//const void*是通用类型指针,size指字节数
{
    GLCall(glGenBuffers(1, &m_RendererID));//生成缓冲区,然后把缓存区的id值传给buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));//绑定ARRAY缓冲区
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));//把数据传给缓冲区,GL_STATIC_DRAW意思是静态数据，修改一次多次使用
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));//绑定ARRAY缓冲区
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));//绑定ARRAY缓冲区
}
