#include "ShaderStorageBuffer.h"
#include <iostream>

#include "Renderer.h"

ShaderStorageBuffer::ShaderStorageBuffer(const void* data, unsigned int size, unsigned int bindingPoint)
    : m_Size(size), m_BindingPoint(bindingPoint)
{
    GLCall(glGenBuffers(1, &m_RendererID)); // ���ɻ���������
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID)); // �󶨻������� SSBO Ŀ��
    GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW)); // �����ڴ沢��ʼ������
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_RendererID)); // ���������󶨵�ָ���İ󶨵�
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // �����
}

ShaderStorageBuffer::~ShaderStorageBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID)); // ɾ������������
}

void ShaderStorageBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID)); // �󶨻�����
}

void ShaderStorageBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // �����
}

void ShaderStorageBuffer::SetData(const void* data, unsigned int size, unsigned int offset)
{
    Bind();
    GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data)); // ���»�����������
    Unbind();
}