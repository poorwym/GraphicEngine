
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

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Camera& camera, Shader& shader, glm::mat4 model) const
{
    va.Bind();//绑定vao

    ib.Bind();//绑定ib

    shader.Bind();
    glm::mat4 proj = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 mvp = proj * view * model;
    shader.setUniformVec3f("viewPos", camera.GetPosition());
    shader.setUniformMat4f("u_Model", model);
    shader.setUniformMat4f("u_View", view);
    shader.setUniformMat4f("u_MVP", mvp);

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));//画三角形，ib.GetCount为indices的长度，GL_UNSIGNED_INT表示索引的类型,nullptr表示索引的指针(因为已经绑定了ibo所以不需要任何指针
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

