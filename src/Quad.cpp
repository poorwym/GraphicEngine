// Quad.cpp
#include "Quad.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include<vector>
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
    GLCall(glDrawElements(GL_TRIANGLES, m_IBO->GetCount(), GL_UNSIGNED_INT, nullptr));// »æÖÆÈý½ÇÐÎ
    m_IBO->Unbind();
    m_VAO->Unbind();
    shader.Unbind();
}