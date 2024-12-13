
#include "Renderer.h"
#include <iostream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError() {//确保之前没有错误，否则会死循环
    while (glGetError() != GL_NO_ERROR) {
        ;
    }
}

bool GLLogCall(const char* function, const char* file, int line) {//打印错误信息
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
    va.Bind(); // 绑定 VAO
    ib.Bind(); // 绑定索引缓冲
    shader.Bind();
    // 如果有相机，设置相机相关的 Uniform
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
        // 如果没有相机，只设置模型矩阵
        shader.SetUniformMat4f("u_MVP", model);
    }

    // 始终设置模型矩阵
    shader.SetUniformMat4f("u_Model", model);

    // 绘制图元
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));// 绘制三角形

    va.Unbind();
    ib.Unbind();
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib,  Shader& shader) const
{
    const int maxIndicesPerBatch = 3000;
    int totalIndices = ib.GetCount();

    va.Bind();   // 绑定 VAO
    ib.Bind();   // 绑定 IBO
    shader.Bind(); // 绑定着色器

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));// 绘制三角形

    va.Unbind();    // 解绑 VAO
    ib.Unbind();    // 解绑 IBO
    shader.Unbind();// 如果有必要，可以添加解绑着色器的代码
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

