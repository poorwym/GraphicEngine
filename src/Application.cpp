//version v1.1
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"

#include "IndexBuffer.h"

#include "VertexArray.h"

#include "Shader.h"

#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "test/TestClearColor.h"
#include "test/TestTexture2D.h"
#include "Camera.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//����OpenGL�汾���汾�� 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);//����OpenGL�汾�ΰ汾��
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//����ʹ�ú���ģʽ


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLCall(glfwSwapInterval(1));

    if (glewInit() != GLEW_OK) {
        std::cout << "error";
    }
    std::cout << glGetString(GL_VERSION) << std::endl;

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));

    /* Loop until the user closes the window */
    //ImGui initialization
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); // ��ȡ io ����

    // ���Ĭ������
    io.Fonts->AddFontDefault();

    // ���� ImGui ��ʽ
    ImGui::StyleColorsDark();

    // ��ʼ��ƽ̨/��Ⱦ��
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // ȷ��������� OpenGL �汾�޸�

    //Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    // �����ӿڿ��
    float width = 1920.0f;
    float height = 1080.0f;
    float aspect_ratio = width / height;

    // ������Ұ�Ƕȣ��Ի���Ϊ��λ������ƽ���Զƽ��
    float fov = glm::radians(45.0f); // 45����Ұ��
    float near_plane = 0.1f;
    float far_plane = 100.0f;

    Camera camera(fov, aspect_ratio, near_plane, far_plane);

    Renderer renderer;
    test::Test* currentTest = nullptr;
    test::TestMenu* testMenu = new test::TestMenu(currentTest);// ��ʼ���˵�

    currentTest = testMenu;
    testMenu->RegisterTest<test::TestClearColor>("Clear Color");//��������
    testMenu->RegisterTest<test::TestTexture2D>("Texture2D");

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        renderer.Clear();
        ImGui_ImplGlfw_NewFrame();  // ���磬�����ʹ�� GLFW
        ImGui_ImplOpenGL3_NewFrame(); // �����ʹ�� OpenGL ��Ϊ��Ⱦ���
        ImGui::NewFrame(); // ImGui �������֡����


        if (currentTest) {
            currentTest->OnUpdate(0.0f);
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<-")) {
                delete currentTest;
                currentTest = testMenu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }

        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    //glDeleteProgram(shader);
    delete currentTest;//��ֹ�ڴ�й©
    if (currentTest != testMenu) {
        delete testMenu;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}