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
#include "EntityController.h"
#include "CameraController.h"
#include "color.h"
#include "Light.h"
#include "LightController.h"
#include "SceneManager.h"
#include<map>
DirectionalLightController directionalLightController;


float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 相机控制器和资源管理器
CameraController* cameraController = nullptr;
ResourceManager resourceManager;

void test2D(GLFWwindow* window);
void test3D(GLFWwindow* window);

// 鼠标移动回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (cameraController)
        cameraController->ProcessMouseInput(static_cast<float>(xpos), static_cast<float>(ypos));
}

// 鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (cameraController)
        cameraController->ProcessMouseScroll(static_cast<float>(yoffset));
}


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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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
    ImGui::GetIO().FontGlobalScale = 1.5f; // 将字体放大到原来的1.5倍

    

    test3D(window);
}

void test3D(GLFWwindow* window) {

    Texture* diffuseMap = resourceManager.Load<Texture>("res/Textures/example.png");
    Mesh* mesh = new Mesh(vertices, indices, new Material(diffuseMap, diffuseMap, diffuseMap));
    Scene* scene = new Scene();
    SceneManager sceneManager(scene);

    sceneManager.AddEntity(mesh,"My First Entity", "node1", nullptr);
    sceneManager.AddEntity(mesh,"My Second Entity", "node2", nullptr);
    sceneManager.AddEntity(mesh, "My Third Entity", "node3", sceneNodeList["node1"]);
    sceneManager.AddPointLight(new PointLight("PointLight", _WHITE, 1.0f, glm::vec3(1.0f)), "node4", sceneNodeList["node1"]);

    Shader* shader = resourceManager.Load<Shader>("res/shaders/light.shader");

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
    cameraController = new CameraController(&camera, window);

    
    DirectionalLight* light = new DirectionalLight("Directional Light", _WHITE, 1.0f, glm::vec3(1.0f));
    directionalLightController = DirectionalLightController(light);

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
         // 获取当前帧的时间
        float currentFrame = glfwGetTime();
        // 计算 deltaTime
        deltaTime = currentFrame - lastFrame;
        // 更新 lastFrame 为当前帧的时间
        lastFrame = currentFrame;

        //ImGui 初始化
        ImGui_ImplGlfw_NewFrame();  // 例如，如果你使用 GLFW
        ImGui_ImplOpenGL3_NewFrame(); // 如果你使用 OpenGL 作为渲染后端
        ImGui::NewFrame(); // ImGui 自身的新帧调用

        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

        scene->OnImGuiTree();

        cameraController->Update(deltaTime);

        shader->Bind();
        shader->setUniform1i("numPointLights", pointLightID.size());

        scene->SetDirectionalLight(light);
        scene->BindLight(*shader, glm::mat4(1.0f));
        scene->Render(*shader, camera);
        scene->Update(0.0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

}