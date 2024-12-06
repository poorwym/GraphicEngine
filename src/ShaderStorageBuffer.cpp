// ShaderStorageBuffer.cpp

#include "ShaderStorageBuffer.h"
#include <iostream>

// ��������һ�� Renderer.h�����ж����� GLCall ��
#include "Renderer.h"

ShaderStorageBuffer::ShaderStorageBuffer()
{
}

// ���캯�����������������ϴ�����
ShaderStorageBuffer::ShaderStorageBuffer(const void* data, unsigned int size, unsigned int bindingPoint)
    : m_Size(size), m_BindingPoint(bindingPoint)
{
    GLCall(glGenBuffers(1, &m_RendererID)); // ���ɻ���������
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID)); // �󶨻������� SSBO Ŀ��
    GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW)); // �����ڴ沢��ʼ������
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_RendererID)); // ���������󶨵�ָ���İ󶨵�
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // �����
}

// �ƶ����캯��
ShaderStorageBuffer::ShaderStorageBuffer(ShaderStorageBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID), m_Size(other.m_Size), m_BindingPoint(other.m_BindingPoint)
{
    other.m_RendererID = 0;
    other.m_Size = 0;
    other.m_BindingPoint = 0;
}

// �ƶ���ֵ�����
ShaderStorageBuffer& ShaderStorageBuffer::operator=(ShaderStorageBuffer&& other) noexcept
{
    if (this != &other)
    {
        // ��ɾ����ǰ�Ļ�����
        GLCall(glDeleteBuffers(1, &m_RendererID));

        // �ƶ�����
        m_RendererID = other.m_RendererID;
        m_Size = other.m_Size;
        m_BindingPoint = other.m_BindingPoint;

        // ������������
        other.m_RendererID = 0;
        other.m_Size = 0;
        other.m_BindingPoint = 0;
    }
    return *this;
}

// ����������ɾ��������
ShaderStorageBuffer::~ShaderStorageBuffer()
{
    if (m_RendererID != 0)
    {
        GLCall(glDeleteBuffers(1, &m_RendererID)); // ɾ������������
    }
}

// �󶨻�����
void ShaderStorageBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID)); // �󶨻�����
}

// ��󻺳���
void ShaderStorageBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0)); // �����
}

// ���»��������ݣ����ָ��£�
void ShaderStorageBuffer::SetData(const void* data, unsigned int size, unsigned int offset) const
{
    Bind();
    GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data)); // ���»�����������
    Unbind();
}

// ӳ�仺������CPU�ڴ�
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

// ���ӳ�仺����
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