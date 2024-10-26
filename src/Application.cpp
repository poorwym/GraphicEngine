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
#include "Scene.h"
#include "ResourseManager.h"

void test2D(GLFWwindow* window);
void test3D(GLFWwindow* window);

std::vector<Vertex> vertices = {
    // 前面
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 前上左
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // 前上右
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 前下左
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 前下右

    // 后面
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 后上左
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // 后上右
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 后下左
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // 后下右

    // 左面
    {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 后上左
    {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 前上左
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 后下左
    {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 前下左

    // 右面
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 后上右
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 前上右
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 后下右
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 前下右

    // 上面
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // 后上左
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, // 后上右
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // 前上左
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // 前上右

    // 下面
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // 后下左
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // 后下右
    {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 前下左
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // 前下右
};

// 立方体的索引
std::vector<unsigned int> indices = {
    // 前面
    0, 1, 2,
    2, 1, 3,

    // 后面
    4, 5, 6,
    6, 5, 7,

    // 左面
    8, 9, 10,
    10, 9, 11,

    // 右面
    12, 13, 14,
    14, 13, 15,

    // 上面
    16, 17, 18,
    18, 17, 19,

    // 下面
    20, 21, 22,
    22, 21, 23,
};

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//设置OpenGL版本主版本号 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);//设置OpenGL版本次版本号
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//设置使用核心模式


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
    ImGuiIO& io = ImGui::GetIO(); // 获取 io 对象

    // 添加默认字体
    io.Fonts->AddFontDefault();

    // 设置 ImGui 样式
    ImGui::StyleColorsDark();

    // 初始化平台/渲染绑定
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // 确保根据你的 OpenGL 版本修改


    //test2D(window);
    test3D(window);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void test2D(GLFWwindow* window) {
    Renderer renderer;
    test::Test* currentTest = nullptr;
    test::TestMenu* testMenu = new test::TestMenu(currentTest);// 初始化菜单

    currentTest = testMenu;
    testMenu->RegisterTest<test::TestClearColor>("Clear Color");//创建测试
    testMenu->RegisterTest<test::TestTexture2D>("Texture2D");

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        renderer.Clear();
        ImGui_ImplGlfw_NewFrame();  // 例如，如果你使用 GLFW
        ImGui_ImplOpenGL3_NewFrame(); // 如果你使用 OpenGL 作为渲染后端
        ImGui::NewFrame(); // ImGui 自身的新帧调用


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
    delete currentTest;//防止内存泄漏
    if (currentTest != testMenu) {
        delete testMenu;
    }
}

void test3D(GLFWwindow* window) {
    ResourceManager resourceManager;
    Texture* texture = resourceManager.Load<Texture>("res/Textures/example.png");
    Mesh mesh(vertices, indices, new Material(texture, texture, texture));
    Scene scene;
    Entity entity;
    entity.AddComponent(new MeshComponent(&mesh));
    SceneNode node("node1", &entity, nullptr);
    scene.addNode(&node);
    Shader* shader = resourceManager.Load<Shader>("res/shaders/Basic.shader");

    // 定义视口宽高
    float width = 1920.0f;
    float height = 1080.0f;
    float aspect_ratio = width / height;

    // 定义视野角度（以弧度为单位）、近平面和远平面
    float fov = 45.0f; // 45度视野角
    float near_plane = 0.1f;
    float far_plane = 100.0f;

    Camera camera(fov, aspect_ratio, near_plane, far_plane);
    camera.SetPosition(glm::vec3(1, 1, 3));

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        scene.Render(*shader, camera);
        /* Swap front and back buffers */

        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    //glDeleteProgram(shader);

}