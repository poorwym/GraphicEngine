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
#include "SkyBox.h"

#include "test/TestClearColor.h"
#include "test/TestTexture2D.h"
#include "Camera.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "EntityController.h"
#include "CameraController.h"
#include "color.h"
#include "Light.h"
#include "LightController.h"
#include "SceneManager.h"
#include<map>
#include"depthMap.h"
DirectionalLightController directionalLightController;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 相机控制器和资源管理器
CameraController* cameraController = nullptr;
ResourceManager resourceManager;

void testPBR(GLFWwindow* window);
static void ViewPortInit(int width, int height) {
    GLCall(glViewport(0, 0, width, height));
    GLCall(glClear(GL_DEPTH_BUFFER_BIT));
}
// 鼠标移动回调函数
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (cameraController)
        cameraController->ProcessMouseInput(static_cast<float>(xpos), static_cast<float>(ypos));
}

// 鼠标滚轮回调函数
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (cameraController)
        cameraController->ProcessMouseScroll(static_cast<float>(yoffset));
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//设置OpenGL版本主版本号 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置OpenGL版本次版本号
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//设置使用核心模式


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE; // 启用实验性功能以确保现代 OpenGL 功能可用
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
    ImGui::GetIO().FontGlobalScale = 1.5f; // 将字体放大到原来的1.5
    testPBR(window);
}
static void LoadModel(SceneManager& sceneManager) {
    //load sun
    MeshComponent* meshComponent1 = resourceManager.LoadOBJ("res/Obj/RAN_Halloween_Pumpkin_2024_OBJ/RAN Halloween Pumpkin 2024 - OBJ/", "RAN_Halloween_Pumpkin_2024_High_Poly.obj", 3.0f);
    sceneManager.AddEntity(meshComponent1, "Pumpkin", "node1", nullptr);
  
}
static void InitModel() {
    return;
}

void testPBR(GLFWwindow* window) {

    Scene* scene = new Scene();
    SceneManager sceneManager(scene);
    LoadModel(sceneManager);
    InitModel();
    Shader* PBRshader = resourceManager.Load<Shader>("res/shaders/PBRshader.shader");
    Shader* depthShader = resourceManager.Load<Shader>("res/shaders/depth_shader.shader");
    //这段真的非常非常重要，忘记绑定了。
    //sampler2D是一个unsigned int类型，值对应到Texture的slot 来自凌晨5：31的一条注释
    PBRshader->Bind();
    PBRshader->setUniform1i("AlbedoMap", 0);
    PBRshader->setUniform1i("NormalMap", 1);
    PBRshader->setUniform1i("MetallicMap", 2);
    PBRshader->setUniform1i("RoughnessMap", 3);
    PBRshader->setUniform1i("AOMap", 4);
    PBRshader->setUniform1i("EmissionMap", 5);
    PBRshader->setUniform1i("HeightMap", 6);
    PBRshader->setUniform1i("DissolveTextureMap", 8);
    PBRshader->setUniform1i("ShadowMap", 7);
    PBRshader->setUniform1i("SpecularExponentTextureMap", 9);
    PBRshader->Unbind();


    // 定义视口宽高
    float width = WINDOW_WIDTH;
    float height = WINDOW_HEIGHT;
    float aspect_ratio = width / height;

    // 定义视野角度（以弧度为单位）、近平面和远平面
    float fov = 30.0f; // 30度视野角
    float near_plane = 0.1f;
    float far_plane = 100.0f;

    Camera camera(fov, aspect_ratio, near_plane, far_plane);
    camera.SetPosition(glm::vec3(1, 1, 3));
    cameraController = new CameraController(&camera, window);

    DirectionalLight* light = new DirectionalLight("Directional Light", _WHITE, 1.0f, glm::vec3(1.0f), _WHITE, _WHITE, _WHITE);
    directionalLightController = DirectionalLightController(light);
    //FrameBuffer depthFBO(SHADOW_WIDTH, SHADOW_HEIGHT);
    std::vector<std::string> faces
    {
        "res/skybox/star.jpg",
        "res/skybox/star.jpg",
        "res/skybox/star.jpg",
        "res/skybox/star.jpg",
        "res/skybox/star.jpg",
        "res/skybox/star.jpg"
    };
    // 创建天空盒实例
    Skybox skybox(faces);
    CubeMapFBO cubeMapFBO(1024, 1024);
    DepthMapFBO depthMapFBO(WINDOW_WIDTH, WINDOW_HEIGHT);
    DepthMapFBO shadowMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);
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
        skybox.Draw(camera);

        scene->OnImGuiTree();

        cameraController->Update(deltaTime);

        PBRshader->Bind();
        PBRshader->setUniform1i("numPointLights", pointLightID.size());
        PBRshader->Unbind();
        
        scene->SetDirectionalLight(light);
        //render shadow
        ViewPortInit(SHADOW_WIDTH, SHADOW_HEIGHT);
        shadowMapFBO.Bind();
        depthShader->Bind();
        glm::mat4 lightSpaceMatrix = light->ComputeLightSpaceMatrix(glm::vec3(0.0f));
        depthShader->setUniformMat4f("SpaceMatrix", lightSpaceMatrix);
        scene->RenderDepthMap(*depthShader);
        depthShader->Unbind();
        shadowMapFBO.Unbind();
        //bind shadowMap
        shadowMapFBO.BindTexture(7);
        //render visibility
        ViewPortInit(WINDOW_WIDTH, WINDOW_HEIGHT);
        depthMapFBO.Bind();
        depthShader->Bind();
        glm::mat4 SpaceMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
        depthShader->setUniformMat4f("SpaceMatrix", SpaceMatrix);
        scene->RenderDepthMap(*depthShader);
        depthShader->Unbind();
        depthMapFBO.Unbind();
        //bind visibilityMap
        depthMapFBO.BindTexture(10);
        //render
        PBRshader->Bind();
        PBRshader->setUniform1i("ShadowMap", 7);
        PBRshader->setUniform1i("ViewDepthMap", 10);
        PBRshader->setUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
        scene->BindLight(*PBRshader, glm::mat4(1.0f));
        scene->Render(*PBRshader, camera);
        scene->Update(deltaTime);
        PBRshader->Unbind();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}
