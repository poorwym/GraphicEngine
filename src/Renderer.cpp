
#include "Renderer.h"
#include <iostream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError() {//ȷ��֮ǰû�д��󣬷������ѭ��
    while (glGetError() != GL_NO_ERROR) {
        ;
    }
}

bool GLLogCall(const char* function, const char* file, int line) {//��ӡ������Ϣ
    while (GLenum error = glGetError()) {
        std::cout << "GL Error: " << error << std::endl;
        std::cout << "in Function: " << function << std::endl;
        std::cout << "in File: " << file << std::endl;
        std::cout << "in Line: " << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    va.Bind();//��vao

    ib.Bind();//��ib

    shader.Bind();


    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));//�������Σ�ib.GetCountΪindices�ĳ��ȣ�GL_UNSIGNED_INT��ʾ����������,nullptr��ʾ������ָ��(��Ϊ�Ѿ�����ibo���Բ���Ҫ�κ�ָ��
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}
