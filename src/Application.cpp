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

float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

// �������������Դ������
CameraController* cameraController = nullptr;
ResourceManager resourceManager;

void testPBR(GLFWwindow* window);
static void ViewPortInit(int width, int height) {
    GLCall(glViewport(0, 0, width, height));
    GLCall(glClear(GL_DEPTH_BUFFER_BIT));
}
// ����ƶ��ص�����
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (cameraController)
        cameraController->ProcessMouseInput(static_cast<float>(xpos), static_cast<float>(ypos));
}

// �����ֻص�����
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
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//����OpenGL�汾���汾�� 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//����OpenGL�汾�ΰ汾��
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//����ʹ�ú���ģʽ


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE; // ����ʵ���Թ�����ȷ���ִ� OpenGL ���ܿ���
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
    ImGuiIO& io = ImGui::GetIO(); // ��ȡ io ����

    // ���Ĭ������
    io.Fonts->AddFontDefault();

    // ���� ImGui ��ʽ
    ImGui::StyleColorsDark();

    // ��ʼ��ƽ̨/��Ⱦ��
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // ȷ��������� OpenGL �汾�޸�
    ImGui::GetIO().FontGlobalScale = 1.5f; // ������Ŵ�ԭ����1.5
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
    //�����ķǳ��ǳ���Ҫ�����ǰ��ˡ�
    //sampler2D��һ��unsigned int���ͣ�ֵ��Ӧ��Texture��slot �����賿5��31��һ��ע��
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


    // �����ӿڿ��
    float width = WINDOW_WIDTH;
    float height = WINDOW_HEIGHT;
    float aspect_ratio = width / height;

    // ������Ұ�Ƕȣ��Ի���Ϊ��λ������ƽ���Զƽ��
    float fov = 30.0f; // 30����Ұ��
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
    // ������պ�ʵ��
    Skybox skybox(faces);
    CubeMapFBO cubeMapFBO(1024, 1024);
    DepthMapFBO depthMapFBO(WINDOW_WIDTH, WINDOW_HEIGHT);
    DepthMapFBO shadowMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
         // ��ȡ��ǰ֡��ʱ��
        float currentFrame = glfwGetTime();
        // ���� deltaTime
        deltaTime = currentFrame - lastFrame;
        // ���� lastFrame Ϊ��ǰ֡��ʱ��
        lastFrame = currentFrame;

        //ImGui ��ʼ��
        ImGui_ImplGlfw_NewFrame();  // ���磬�����ʹ�� GLFW
        ImGui_ImplOpenGL3_NewFrame(); // �����ʹ�� OpenGL ��Ϊ��Ⱦ���
        ImGui::NewFrame(); // ImGui �������֡����

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
