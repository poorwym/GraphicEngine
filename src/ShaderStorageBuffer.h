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
    // ���캯�����������������ϴ�����
    ShaderStorageBuffer(const void* data, unsigned int size, unsigned int bindingPoint);

    // ����������ɾ��������
    ~ShaderStorageBuffer();

    // �󶨻�����
    void Bind() const;

    // ��󻺳���
    void Unbind() const;

    // ���»��������ݣ����ָ��£�
    void SetData(const void* data, unsigned int size, unsigned int offset = 0) const;

    // ӳ�仺������CPU�ڴ�
    void* Map(GLenum access = GL_READ_ONLY) const;

    // ���ӳ�仺����
    void Unmap() const;

    // ��ȡ������ID
    GLuint GetID() const { return m_RendererID; }

    // ��ȡ��������С
    GLsizeiptr GetSize() const { return m_Size; }

    // ��ȡ�󶨵�
    GLuint GetBindingPoint() const { return m_BindingPoint; }

    // ��ֹ��������Ϳ�����ֵ
    ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;
    ShaderStorageBuffer& operator=(const ShaderStorageBuffer&) = delete;

    // �����ƶ�������ƶ���ֵ
    ShaderStorageBuffer(ShaderStorageBuffer&& other) noexcept;
    ShaderStorageBuffer& operator=(ShaderStorageBuffer&& other) noexcept;
};