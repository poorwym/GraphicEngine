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
#include "Quad.h"
#include<map>
DirectionalLightController directionalLightController;
ResourceManager resourceManager;
DirectionalLight* light = nullptr;
Shader* mainShader = nullptr;
Shader* depthShader = nullptr;
Shader* cubeDepthShader = nullptr;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastTime = 0.0f; // 上一帧的时间



void testPBR(Scene* scene, Camera& camera, CameraController* cameraController, DepthMapFBO& depthFBO);
static void InitModel();
static void LoadModel(SceneManager& sceneManager);
static void ViewPortInit(int width, int height) {
    GLCall(glViewport(0, 0, width, height));
    GLCall(glClear(GL_DEPTH_BUFFER_BIT));
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    CameraController* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    if (controller)
        controller->ProcessMouseInput(static_cast<float>(xpos), static_cast<float>(ypos));
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    CameraController* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    if (controller)
        controller->ProcessMouseScroll(static_cast<float>(yoffset));
}


int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//设置OpenGL版本主版本号 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置OpenGL版本次版本号
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//设置使用核心模式

    glewExperimental = GL_TRUE; // 启用实验性功能以确保现代 OpenGL 功能可用

    // 创建第一个窗口并设为当前上下文
    GLFWwindow* window1 = glfwCreateWindow(800, 600, "Window 1", NULL, NULL);
    if (!window1) {
        std::cout << "Failed to create window1\n";
        return -1;
    }
    glfwMakeContextCurrent(window1);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW initialization failed for window1\n";
        return -1;
    }

    // 创建其他窗口，启用上下文共享
    GLFWwindow* window2 = glfwCreateWindow(800, 600, "Window 2", NULL, window1);
    if (!window2) {
        std::cout << "Failed to create window2\n";
        return -1;
    }
    GLFWwindow* window3 = glfwCreateWindow(800, 600, "Window 3", NULL, window1);
    if (!window3) {
        std::cout << "Failed to create window3\n";
        return -1;
    }
    GLFWwindow* window4 = glfwCreateWindow(800, 600, "Window 4", NULL, window1);
    if (!window4) {
        std::cout << "Failed to create window4\n";
        return -1;
    }

    // 初始化 OpenGL 设置和资源
    // 注意：只需要在第一个窗口的上下文中初始化一次
    GLCall(glfwSwapInterval(1));

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));

    glEnable(GL_FRAGMENT_DEPTH);

    
    mainShader = resourceManager.Load<Shader>("res/shaders/PBRshader.shader");
    depthShader = resourceManager.Load<Shader>("res/shaders/depth_shader.shader");
    cubeDepthShader = resourceManager.Load<Shader>("res/shaders/cubeMapDepth.shader");

    Scene* scene1 = new Scene();
    light = new DirectionalLight("Directional Light", _WHITE, 1.0f, glm::vec3(1.0f));
    SceneManager sceneManager1(scene1);
    LoadModel(sceneManager1);
    InitModel();



    float nearClip = 0.1f; // 近裁剪面距离
    float FOVx = 30.0f; // 每个相机的水平视野角度
    float FOVx_rad = glm::radians(FOVx / 2.0f);
    float width = nearClip * tan(FOVx_rad);
    float aspectRatio = 4.0f / 3.0f; 
    float height = width / aspectRatio;
    // 为每个窗口创建相机和控制器
    Camera camera1(-width, 0.0f, 0.0f, height, nearClip, 100.0f);
    CameraController* cameraController1 = new CameraController(&camera1, window1);
    glfwSetWindowUserPointer(window1, cameraController1);
    glfwSetCursorPosCallback(window1, mouse_callback);
    glfwSetScrollCallback(window1, scroll_callback);

    glfwMakeContextCurrent(window2);
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));

    glEnable(GL_FRAGMENT_DEPTH);
    Scene* scene2 = new Scene();
    light = new DirectionalLight("Directional Light", _WHITE, 1.0f, glm::vec3(1.0f));
    SceneManager sceneManager2(scene2);
    LoadModel(sceneManager2);
    InitModel();
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW initialization failed for window2\n";
        return -1;
    }
    Camera camera2(0.0f, width, 0.0f, height, nearClip, 100.0f);
    CameraController* cameraController2 = new CameraController(&camera2, window2);
    glfwSetWindowUserPointer(window2, cameraController2);
    glfwSetCursorPosCallback(window2, mouse_callback);
    glfwSetScrollCallback(window2, scroll_callback);

    glfwMakeContextCurrent(window3);
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));

    glEnable(GL_FRAGMENT_DEPTH);
    Scene* scene3 = new Scene();
    light = new DirectionalLight("Directional Light", _WHITE, 1.0f, glm::vec3(1.0f));
    SceneManager sceneManager3(scene3);
    LoadModel(sceneManager3);
    InitModel();
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW initialization failed for window3\n";
        return -1;
    }
    Camera camera3(-width, 0.0f, -height, 0.0f, nearClip, 100.0f);
    CameraController* cameraController3 = new CameraController(&camera3, window3);
    glfwSetWindowUserPointer(window3, cameraController3);
    glfwSetCursorPosCallback(window3, mouse_callback);
    glfwSetScrollCallback(window3, scroll_callback);

    glfwMakeContextCurrent(window4);
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));

    glEnable(GL_FRAGMENT_DEPTH);
    Scene* scene4 = new Scene();
    light = new DirectionalLight("Directional Light", _WHITE, 1.0f, glm::vec3(1.0f));
    SceneManager sceneManager4(scene4);
    LoadModel(sceneManager4);
    InitModel();
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW initialization failed for window4\n";
        return -1;
    }
    Camera camera4(0.0f, width, -height, 0.0f, nearClip, 100.0f);
    CameraController* cameraController4 = new CameraController(&camera4, window4);
    glfwSetWindowUserPointer(window4, cameraController4);
    glfwSetCursorPosCallback(window4, mouse_callback);
    glfwSetScrollCallback(window4, scroll_callback);
    
    // 在第一个上下文中初始化 OpenGL 设置
    glfwMakeContextCurrent(window1);
    camera1.SetPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    camera2.SetPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    camera3.SetPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    camera4.SetPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window1) || !glfwWindowShouldClose(window2) ||
        !glfwWindowShouldClose(window3) || !glfwWindowShouldClose(window4)) {

        // 窗口 1 渲染
        glfwMakeContextCurrent(window1);
        DepthMapFBO depthMapFBO1(WINDOW_WIDTH, WINDOW_HEIGHT);
        testPBR(scene1, camera1, cameraController1, depthMapFBO1);
        glfwSwapBuffers(window1);

        // 窗口 2 渲染
        glfwMakeContextCurrent(window2);
        DepthMapFBO depthMapFBO2(WINDOW_WIDTH, WINDOW_HEIGHT);
        testPBR(scene2, camera2, cameraController2, depthMapFBO2);
        glfwSwapBuffers(window2);

        // 窗口 3 渲染
        glfwMakeContextCurrent(window3);
        DepthMapFBO depthMapFBO3(WINDOW_WIDTH, WINDOW_HEIGHT);
        testPBR(scene3, camera3, cameraController3, depthMapFBO3);
        glfwSwapBuffers(window3);

        // 窗口 4 渲染
        glfwMakeContextCurrent(window4);
        DepthMapFBO depthMapFBO4(WINDOW_WIDTH, WINDOW_HEIGHT);
        testPBR(scene4, camera4, cameraController4, depthMapFBO4);
        glfwSwapBuffers(window4);

        // 同步相机位置（如果需要）
        glm::vec3 position = camera1.GetPosition();
        glm::vec3 target = camera1.GetTarget();
        camera2.SetPosition(position);
        camera2.SetTarget(target);
        camera3.SetPosition(position);
        camera3.SetTarget(target);
        camera4.SetPosition(position);
        camera4.SetTarget(target);
        camera1.Update(deltaTime);
        camera2.Update(deltaTime);
        camera3.Update(deltaTime);
        camera4.Update(deltaTime);
        // 处理事件
        glfwPollEvents();
    }

    // 清理资源
    delete cameraController1;
    delete cameraController2;
    delete cameraController3;
    delete cameraController4;
    glfwTerminate();
    return 0;
}
static void LoadModel(SceneManager& sceneManager) {
    //load sun
    MeshComponent* meshComponent1 = resourceManager.LoadOBJ("res/Obj/RAN_Halloween_Pumpkin_2024_OBJ/RAN Halloween Pumpkin 2024 - OBJ/", "RAN_Halloween_Pumpkin_2024_High_Poly.obj", 6.0f);
    sceneManager.AddEntity(meshComponent1, "Pumpkin", "node1", nullptr);
}
static void InitModel() {

}

void testPBR(Scene* scene, Camera& camera, CameraController* cameraController, DepthMapFBO& depthMapFBO) {
    mainShader->Bind();
    mainShader->setUniform1i("AlbedoMap", 0);
    mainShader->setUniform1i("NormalMap", 1);
    mainShader->setUniform1i("MetallicMap", 2);
    mainShader->setUniform1i("RoughnessMap", 3);
    mainShader->setUniform1i("AOMap", 4);
    mainShader->setUniform1i("EmissionMap", 5);
    mainShader->setUniform1i("HeightMap", 6);
    mainShader->setUniform1i("DissolveTextureMap", 8);
    mainShader->setUniform1i("ShadowMap", 7);
    mainShader->setUniform1i("SpecularExponentTextureMap", 9);
    mainShader->setUniform1f("farPlane", 100.0f);
    mainShader->Unbind();
    float curentTime = glfwGetTime();
    // 计算 deltaTime
    float deltaTime = curentTime - lastTime;
    // 更新 lastTime 为当前帧的时间
    lastTime = curentTime;

    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

    cameraController->Update(deltaTime);

    mainShader->Bind();
    mainShader->setUniform1i("numPointLights", pointLightID.size());
    mainShader->Unbind();

    scene->SetDirectionalLight(light);
    scene->RenderShadowMap(depthShader, cubeDepthShader);

    //render visibility
    ViewPortInit(WINDOW_WIDTH, WINDOW_HEIGHT);
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
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
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    mainShader->Bind();
    mainShader->setUniform1i("ShadowMap", 7);
    mainShader->setUniform1i("ViewDepthMap", 10);
    for (int i = 0; i < 4; i++)
    {
        mainShader->setUniform1i("PointShadowMap[" + std::to_string(i) + "]", 11 + i);
    }
    mainShader->setUniformMat4f("lightSpaceMatrix", light->ComputeLightSpaceMatrix(glm::vec3(0.0f)));
    scene->BindLight(*mainShader, glm::mat4(1.0f));
    scene->Render(*mainShader, camera);
    mainShader->Unbind();

    scene->Update(deltaTime);
}