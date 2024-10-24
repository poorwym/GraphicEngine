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
        //����ʹ��ָ��һ������Ҫ��ԭ���ǣ�����ָ��������������ᱻ���ã�������Ҫ�ֶ��ͷ�
        float position[] = {
       -100.0f, 100.0f, 0.0f, 0.0f,//0 ����λ��ʾ��ͼ������
       -100.0f, -100.0f, 1.0f, 0.0f,//1
       100.0f, -100.0f, 1.0f, 1.0f,//2
       100.0f, 100.0f, 0.0f, 1.0f //3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        m_Proj = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);//ͶӰ����
        m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));//��ͼ����       
       
        m_VAO = std::make_unique<VertexArray> ();

        //���ģʽ�������������ppt
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VertexBuffer = std::make_unique<VertexBuffer>(position, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);//ÿ��vertex2��float
        layout.Push<float>(2);//ÿ��vertex2��float�����Ӧ���Ǻ���ͼ�й�

        m_IndexBuffer = std::make_unique<IndexBuffer>(indices,6);

        m_VAO -> AddBuffer(*m_VertexBuffer, layout);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader -> Bind();
        m_Shader->setUniform4f("u_Color", 0.0f, 0.0f, 0.0f, 1.0f);
        m_Texture = std::make_unique<Texture>("res/Textures/card_box_bottom.png");
        m_Shader->setUniform1i("u_Texture", 0);//0��ʾtexture slot��Ĭ����0��
        //shader.setUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        
  
        //������а�
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
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);//ģ�;���
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader -> Bind();
            m_Shader -> setUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);//ģ�;���
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
