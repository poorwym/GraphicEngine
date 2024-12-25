// Quad.cpp
#include "Quad.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include <vector>
#include <iostream>
//#include "NGFX_Injection.h"
#include "GLFW/glfw3.h"
//#include "NVIDIA_Nsight.h"
#include <thread>
#include <chrono>

//extern NsightGraphicsManager& g_NsightGraphicsManager;

float quadVertices[] = {
        // positions        // texCoords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // bottom left 
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // top left
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // top right
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f  // bottom right
    };

std::vector<unsigned int> indices = {
    1, 3, 2,
    1, 3, 0
};
Quad::Quad()
{
    m_VAO = new VertexArray();
    m_VBO = new VertexBuffer(quadVertices, 25 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_VAO->Bind();
    m_VAO->AddBuffer(*m_VBO, layout);
    m_IBO = new IndexBuffer(indices.data(), indices.size());
    m_VAO->Unbind();

}

Quad::~Quad()
{
    delete m_IBO;
    delete m_VAO;
    delete m_VBO;
}

void Quad::Render(Shader& shader) 
{
    shader.Bind();
    m_VAO->Bind();
    m_IBO->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, m_IBO->GetCount(), GL_UNSIGNED_INT, nullptr));// 绘制三角形
    m_IBO->Unbind();
    m_VAO->Unbind();
    shader.Unbind();
}

// Full-screen quad vertices (NDC coordinates)
float simpleQuadVertices[] = {
    // positions
    -1.0f,  1.0f,  // Top-left
    -1.0f, -1.0f,  // Bottom-left
     1.0f, -1.0f,  // Bottom-right

    -1.0f,  1.0f,  // Top-left
     1.0f, -1.0f,  // Bottom-right
     1.0f,  1.0f   // Top-right
};

SimpleQuad::SimpleQuad()
{
    m_VAO = new VertexArray();
    m_VBO = new VertexBuffer(simpleQuadVertices, 12 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    m_VAO->Bind();
    m_VAO->AddBuffer(*m_VBO, layout);
    m_VAO->Unbind();
}

SimpleQuad::SimpleQuad(const std::vector<float>& vertices)
{
    m_VAO = new VertexArray();
    m_VBO = new VertexBuffer(vertices.data(), 12 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    m_VAO->Bind();
    m_VAO->AddBuffer(*m_VBO, layout);
    m_VAO->Unbind();
}

SimpleQuad::~SimpleQuad()
{
    delete m_VAO;
    delete m_VBO;
}

void SimpleQuad::Render(Shader& shader)
{
    shader.Bind();
    m_VAO->Bind();
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
    m_VAO->Unbind();
    shader.Unbind();
}

TileQuad::TileQuad(int n) 
    :m_VAO(nullptr), m_VBO(nullptr)
{
    float width = 2.0f / static_cast<float>(n);  // 每个小块的宽度
    float height = 2.0f / static_cast<float>(n); // 每个小块的高度

    // 为每个小块创建顶点数据并存储在 m_Quads 中
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float x_offset = -1.0f + j * width;  // 每个小块的 x 偏移量
            float y_offset = -1.0f + i * height; // 每个小块的 y 偏移量

            // 为当前小块生成顶点数据（每个小块由2个三角形组成）
            std::vector<float> vertices = {
                // positions
                x_offset, y_offset,  // Top-left
                x_offset, y_offset + height,  // Bottom-left
                x_offset + width, y_offset + height,  // Bottom-right

                x_offset, y_offset,  // Top-left
                x_offset + width, y_offset + height,  // Bottom-right
                x_offset + width, y_offset  // Top-right
            };

            // 创建 SimpleQuad 来表示当前小块
            SimpleQuad* quad = new SimpleQuad(vertices);
            m_Quads.push_back(quad);
        }
    }
}

TileQuad::~TileQuad() {
    for (auto& quad : m_Quads) {
        delete quad;
    }
}

void TileQuad::Render(Shader& shader, GLFWwindow* window) {
    shader.Bind();
    int count = 0;
    float size = m_Quads.size();
    for (auto& quad : m_Quads) {
        quad->Render(shader);
#ifdef _DEBUG
        //g_NsightGraphicsManager.CaptureFrame();
        //GLCall(glfwSwapBuffers(window));
        GLCall(glFinish());
#endif
        if(count % 100 == 0)
            std::cout << "Rendered " << static_cast<float>(count)/size * 100 << " %" << std::endl;
        count++;
    }
    shader.Unbind();
}