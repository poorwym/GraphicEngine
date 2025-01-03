#pragma once

#include<vector>
#include<GL/glew.h>
#include "Macro.h"

struct VertexBufferElement {//基本上对应glVertexAttribPointer的各个参数
    unsigned int type;
    unsigned int count;
    
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
        case GL_FLOAT:          return 4;
        case GL_UNSIGNED_INT:   return 4;
        case GL_INT:            return 4;
        case GL_UNSIGNED_BYTE:  return 1;
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    VertexBufferLayout() :m_Stride(0) {};
    void AddStride(int n) { m_Stride += n; };

    template<typename T>
    void Push(unsigned int count) {
        static_assert(false);//编译时报错
    }

    template<>
    void Push<float>(unsigned int count)
    {
        m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
        m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
    }

    template<>
    void Push<unsigned int>(unsigned int count)
    {
        m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
        m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
    }

    template<>
    void Push<unsigned char>(unsigned int count)
    {
        m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
        m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
    }

    template<>
    void Push<int>(unsigned int count)
    {
        m_Elements.push_back({ GL_INT, count, GL_FALSE });
        m_Stride += VertexBufferElement::GetSizeOfType(GL_INT) * count;
    }

    inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }
};