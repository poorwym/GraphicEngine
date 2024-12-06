#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size) // const void* ��ͨ������ָ�룬size ���ֽ���
{
    GLCall(glGenBuffers(1, &m_RendererID)); // ���ɻ������������� ID �洢�� m_RendererID ��
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // �󶨵� GL_ARRAY_BUFFER Ŀ��
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW)); // ʹ�� GL_DYNAMIC_DRAW ��Ϊ����ʹ��ģʽ
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID)); // ɾ��������
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // �󶨻�����
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // ��󻺳���
}

void VertexBuffer::setData(const void* data, unsigned int size)
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // �󶨻�����

    // ʹ�� glBufferSubData ���»������Ĳ��ֻ�ȫ������
    // ȷ���µ����ݴ�С��������������ԭʼ��С
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));

    // ��󻺳�������ѡ��
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}