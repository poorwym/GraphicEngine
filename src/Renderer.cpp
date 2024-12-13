
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

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Camera* camera, Shader& shader, const glm::mat4& model) const
{
    va.Bind(); // �� VAO
    ib.Bind(); // ����������
    shader.Bind();
    // �������������������ص� Uniform
    if (camera)
    {
        glm::mat4 proj = camera->GetProjectionMatrix();
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 mvp = proj * view * model;

        shader.SetUniformVec3f("viewPos", camera->GetPosition());
        shader.SetUniformMat4f("u_View", view);
        shader.SetUniformMat4f("u_MVP", mvp);
    }
    else
    {
        // ���û�������ֻ����ģ�;���
        shader.SetUniformMat4f("u_MVP", model);
    }

    // ʼ������ģ�;���
    shader.SetUniformMat4f("u_Model", model);

    // ����ͼԪ
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));// ����������

    va.Unbind();
    ib.Unbind();
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib,  Shader& shader) const
{
    const int maxIndicesPerBatch = 3000;
    int totalIndices = ib.GetCount();

    va.Bind();   // �� VAO
    ib.Bind();   // �� IBO
    shader.Bind(); // ����ɫ��

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));// ����������

    va.Unbind();    // ��� VAO
    ib.Unbind();    // ��� IBO
    shader.Unbind();// ����б�Ҫ��������ӽ����ɫ���Ĵ���
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

