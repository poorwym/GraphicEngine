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

float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

// �������������Դ������
CameraController* cameraController = nullptr;
ResourceManager resourceManager;
SceneManager sceneManager(nullptr);

void PBR_Render(Camera& camera, Scene* scene);
void RealTimeRender(GLFWwindow* window);
static void RenderFBOtoScreen(ColorFBO& colorFBO);
static ColorFBO PostRender(ColorFBO& colorFBO, Camera& camera);
static void ViewPortInit(int width, int height) {
    GLCall(glViewport(0, 0, width, height));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
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
    GLint maxLayers;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxLayers);
    std::cout << "Max TextureArray layers: " << maxLayers << std::endl;

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
    RealTimeRender(window);
}
static void LoadModel(SceneManager& sceneManager) {
    //load sun
    MeshComponent* meshComponent1 = resourceManager.LoadOBJ("res/Obj/OBJ_2247/", "OBJ_2247.obj", 0.3f);
    sceneManager.AddEntity(meshComponent1, "Pumpkin", "node1", nullptr);
    PointLight* pointLight = new PointLight("PointLight", _WHITE, 1.0f, glm::vec3(0.0f, 0.5f, 0.0f));
    sceneManager.AddPointLight(pointLight, "node2", nullptr);
}
static void InitModel() {

}

static void InitCamera(Camera& camera) {
    camera.SetPosition(glm::vec3(9.670f, 2.000f, 7.819f));
    camera.SetTarget(glm::vec3(5.884, 0.175, 4.545));
    camera.SetFocus(0.2f, 1.0f, 0.05);
}
void RealTimeRender(GLFWwindow* window) {
    // �����ӿڿ��
    float width = WINDOW_WIDTH;
    float height = WINDOW_HEIGHT;
    float aspect_ratio = width / height;


    // ������Ұ�Ƕȣ��Ի���Ϊ��λ������ƽ���Զƽ��
    float fov = 30.0f; // 30����Ұ��
    float near_plane = NEAR_PLANE;
    float far_plane = FAR_PLANE;

    Camera camera(fov, aspect_ratio, near_plane, far_plane);
    InitCamera(camera);
    cameraController = new CameraController(&camera, window);

    Scene* scene = new Scene();
    sceneManager = SceneManager(scene);
    LoadModel(sceneManager);
    InitModel();

    Shader* mainShader = resourceManager.Load<Shader>("res/shaders/Batch.shader");
    Shader* depthShader = resourceManager.Load<Shader>("res/shaders/depth_shader.shader");
    Shader* cubeDepthShader = resourceManager.Load<Shader>("res/shaders/cubeMapDepth.shader");
    ColorFBO colorFBO(WINDOW_WIDTH, WINDOW_HEIGHT);
    //�����ķǳ��ǳ���Ҫ�����ǰ��ˡ�
    //sampler2D��һ��unsigned int���ͣ�ֵ��Ӧ��Texture��slot �����賿5��31��һ��ע��
    mainShader->Bind();
    mainShader->setUniform1f("farPlane", far_plane);
    mainShader->setUniform1i("ShadowMap", 31);
    mainShader->Unbind();


    DirectionalLight* light = new DirectionalLight("Directional Light", _DARKGREY, 3.0f, glm::vec3(1.0f));
    scene->SetDirectionalLight(light);
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
    DepthMapFBO depthMapFBO(WINDOW_WIDTH, WINDOW_HEIGHT);
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

        scene->OnImGuiTree();
        cameraController->OnImGuiRender();
        cameraController->Update(deltaTime);
        mainShader->Bind();
        mainShader->setUniform1i("numPointLights", pointLightID.size());
        mainShader->Unbind();

        ViewPortInit(SHADOW_WIDTH, SHADOW_HEIGHT);
        scene->RenderShadowMap(depthShader, cubeDepthShader);

        //render
        ViewPortInit(WINDOW_WIDTH, WINDOW_HEIGHT);
        colorFBO.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.Draw(camera);
        mainShader->Bind();
        mainShader->setUniform1i("ShadowMap", 31);
        for (int i = 0; i < 4; i++)
        {
            mainShader->setUniform1i("PointShadowMap[" + std::to_string(i) + "]", 27 + i);
        }
        scene->BindLight(*mainShader, glm::mat4(1.0f));
        scene->BatchRender(*mainShader, camera);
        mainShader->Unbind();
        colorFBO.Unbind();
        //post render
        ColorFBO t = PostRender(colorFBO, camera);
        RenderFBOtoScreen(t);
        //update
        scene->Update(deltaTime);
        

        ImGui::Begin("PBRrender");
        if (ImGui::Button("Render")) {
            PBR_Render(camera, scene);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }
}



void PBR_Render(Camera& camera, Scene* scene) {

    Shader* mainShader = resourceManager.Load<Shader>("res/shaders/PBRshader.shader");
    Shader* depthShader = resourceManager.Load<Shader>("res/shaders/depth_shader.shader");
    Shader* cubeDepthShader = resourceManager.Load<Shader>("res/shaders/cubeMapDepth.shader");
    ColorFBO colorFBO(WINDOW_WIDTH, WINDOW_HEIGHT);
    //�����ķǳ��ǳ���Ҫ�����ǰ��ˡ�
    //sampler2D��һ��unsigned int���ͣ�ֵ��Ӧ��Texture��slot �����賿5��31��һ��ע��
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
    mainShader->setUniform1f("farPlane", FAR_PLANE);
    mainShader->setUniform1i("AlphaMap", 15);
    mainShader->Unbind();

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
    DepthMapFBO depthMapFBO(WINDOW_WIDTH, WINDOW_HEIGHT);

    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

    mainShader->Bind();
    mainShader->setUniform1i("numPointLights", pointLightID.size());
    mainShader->Unbind();

    ViewPortInit(SHADOW_WIDTH, SHADOW_HEIGHT);
    scene->RenderShadowMap(depthShader, cubeDepthShader);

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
    colorFBO.Bind();
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    skybox.Draw(camera);
    mainShader->Bind();
    mainShader->setUniform1i("ShadowMap", 7);
    mainShader->setUniform1i("ViewDepthMap", 10);
    for(int i=0; i < 4; i++)
    {
        mainShader->setUniform1i("PointShadowMap["+std::to_string(i) + "]", 11 + i);
    }
    scene->BindLight(*mainShader, glm::mat4(1.0f));
    scene->Render(*mainShader, camera);
    mainShader->Unbind();
    colorFBO.Unbind();
    //post render
    ColorFBO finalPicture = PostRender(colorFBO, camera);
    resourceManager.SaveFBOToPNG(colorFBO, "test.png", WINDOW_WIDTH, WINDOW_HEIGHT);
}
static void RenderFBOtoScreen(ColorFBO& colorFBO) {
    Quad screenQuad;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader* screenShader = resourceManager.Load<Shader>("res/shaders/screen.shader");
    colorFBO.BindTexture(0);
    colorFBO.BindDepthTexture(1);
    screenShader->Bind();
    screenShader->setUniform1i("screenTexture", 0);
    //screenShader->setUniform1i("depthTexture", 1);
    screenQuad.Render(*screenShader);
    screenShader->Unbind();
}

static ColorFBO PostRender(ColorFBO& colorFBO, Camera& camera) {
    static Quad screenQuad;
    ColorFBO finalFBO(WINDOW_WIDTH, WINDOW_HEIGHT);

    Shader* FODshader = resourceManager.Load<Shader>("res/shaders/FOD.shader");
    finalFBO.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        colorFBO.BindTexture(0);
        colorFBO.BindDepthTexture(1);
        FODshader->Bind();
            FODshader->setUniform1i("screenTexture", 0);
            FODshader->setUniform1i("depthTexture", 1);
            FODshader->setUniform1f("focusDepth", camera.GetFocusDepth());
            FODshader->setUniform1f("focusRange", camera.GetFocusRange());
            FODshader->setUniform1f("maxBlur", camera.GetMaxBlur());
            screenQuad.Render(*FODshader);
        FODshader->Unbind();
    finalFBO.Unbind();

    return finalFBO;
}