#include "IndexBuffer.h"

#include "Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    : m_Count(count)
{
    GLCall(glGenBuffers(1, &m_RendererID));//���ɻ�����,Ȼ��ѻ�������idֵ����buffer
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));//��ARRAY������
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof( unsigned int ), data, GL_STATIC_DRAW));//�����ݴ���������,GL_STATIC_DRAW��˼�Ǿ�̬���ݣ��޸�һ�ζ��ʹ��
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));//��ARRAY������
}

void IndexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));//��ARRAY������
}
