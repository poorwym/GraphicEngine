#include "TestClearColor.h"
#include "Macro.h"
#include "Renderer.h"
#include "imgui/imgui.h"
#include "TestTexture2D.h"
#include "Texture.h"

namespace test
{
	TestTexture2D::TestTexture2D()
        :m_TranslationA(glm::vec3(200,200,0)),m_TranslationB(glm::vec3(100,400,0)),
        m_Proj(), m_View()
	{
        //这里使用指针一个很重要的原因是，智能指针的析构函数不会被调用，所以需要手动释放
        float position[] = {
       -100.0f, 100.0f, 0.0f, 0.0f,//0 后两位表示贴图的坐标
       -100.0f, -100.0f, 1.0f, 0.0f,//1
       100.0f, -100.0f, 1.0f, 1.0f,//2
       100.0f, 100.0f, 0.0f, 1.0f //3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        m_Proj = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);//投影矩阵
        m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));//视图矩阵       
       
        m_VAO = std::make_unique<VertexArray> ();

        //混合模式，具体见保留的ppt
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VertexBuffer = std::make_unique<VertexBuffer>(position, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);//每个vertex2个float
        layout.Push<float>(2);//每个vertex2个float，这个应该是和贴图有关

        m_IndexBuffer = std::make_unique<IndexBuffer>(indices,6);

        m_VAO -> AddBuffer(*m_VertexBuffer, layout);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader -> Bind();
        m_Shader->setUniform4f("u_Color", 0.0f, 0.0f, 0.0f, 1.0f);
        m_Texture = std::make_unique<Texture>("res/Textures/card_box_bottom.png");
        m_Shader->setUniform1i("u_Texture", 0);//0表示texture slot（默认是0）
        //shader.setUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        
  
        //解除所有绑定
        m_VAO -> Unbind();
        m_VertexBuffer -> Unbind();
        m_IndexBuffer -> Unbind();
        m_Shader -> Unbind();
	}
	TestTexture2D::~TestTexture2D()
	{
	}
	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}
	void TestTexture2D::OnRender()
	{
        Renderer renderer;

        m_Texture -> Bind();
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);//模型矩阵
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader -> Bind();
            m_Shader -> setUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);//模型矩阵
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader -> setUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
	}
	void TestTexture2D::OnImGuiRender()
	{
        ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 1080.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 1080.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}
