#pragma once
#include "Renderer.h"

struct Triangle {
    glm::vec3 v0;               // ���� 0 ��λ��
    glm::vec3 v1;               // ���� 1 ��λ��
    glm::vec3 v2;               // ���� 2 ��λ��
    glm::vec3 normal;           // �淨��
    glm::vec3 tangent;          // ���ߣ����ʹ�÷�����ͼ��
    glm::vec3 bitangent;        // �����ߣ����ʹ�÷�����ͼ��
    glm::vec2 texCoords[3];     // �����������������
    int materialIndex;          // ��������
};

class ShaderStorageBuffer
{
private:
    unsigned int m_RendererID;    // ����������� ID
    unsigned int m_Size;          // ��������С
    unsigned int m_BindingPoint;  // �󶨵�����

public:
    ShaderStorageBuffer(const void* data, unsigned int size, unsigned int bindingPoint);
    ~ShaderStorageBuffer();

    void Bind() const;
    void Unbind() const;

    void SetData(const void* data, unsigned int size, unsigned int offset = 0);

    inline unsigned int GetRendererID() const { return m_RendererID; }
    inline unsigned int GetSize() const { return m_Size; }
    inline unsigned int GetBindingPoint() const { return m_BindingPoint; }
};