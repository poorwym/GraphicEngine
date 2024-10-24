#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)//const void*��ͨ������ָ��,sizeָ�ֽ���
{
    GLCall(glGenBuffers(1, &m_RendererID));//���ɻ�����,Ȼ��ѻ�������idֵ����buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));//��ARRAY������
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));//�����ݴ���������,GL_STATIC_DRAW��˼�Ǿ�̬���ݣ��޸�һ�ζ��ʹ��
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));//��ARRAY������
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));//��ARRAY������
}
