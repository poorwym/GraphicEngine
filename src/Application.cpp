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
#include <map>

#include "opencv2/opencv.hpp"
#include "ShaderStorageBuffer.h"
#include "Filter.h"
#include "BVHTree.h"
#include "TextureManager.h"
#include "TriangleSubdivider.h"
#include "EngineState.h"
#include "Octree.h"
#include "NGFX_Injection.h"
#include "NVIDIA_Nsight.h"

extern TextureManager g_TextureManager;

EngineState engineState;
SceneManager g_SceneManager(nullptr);
DirectionalLightController directionalLightController;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 相机控制器和资源管理器
CameraController* cameraController = nullptr;
ResourceManager resourceManager;


void RayTracing(Camera& camera, Scene* scene, int sampleRate, GLFWwindow* window);
void RealTimeRender(GLFWwindow* window);
static void RenderFBOtoScreen(ColorFBO& colorFBO);
static ColorFBO PostRender(ColorFBO& colorFBO, Camera& camera);
static ColorFBO PostRender(ColorFBO& colorFBO, DepthMapFBO& depthFBO, Camera& camera);

// 视口初始化
static void ViewPortInit(int width, int height) {
    GLCall(glViewport(0, 0, width, height));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
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
extern NsightGraphicsManager& g_NsightGraphicsManager;

// 定义调试回调函数
void APIENTRY OpenGLDebugCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam) {
    // 过滤不重要的消息
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    std::cerr << "OpenGL Debug Message (" << id << "): " << message << std::endl;

    // 根据错误类型和严重性决定是否捕获帧
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        // 获取 NsightGraphicsManager 单例
        NsightGraphicsManager& nsightManager = NsightGraphicsManager::GetInstance();
        if (!nsightManager.CaptureFrame()) {
            std::cerr << "Failed to capture frame on GPU error." << std::endl;
        }
    }
}

int main(void)
{    
    if (!g_NsightGraphicsManager.Initialize()) {
        std::cout << "Nsight Graphics Manager initialization failed." << std::endl;
        return -1;
    }
    GLFWwindow* window;
    // 设置 OpenCV 日志级别为 ERROR，减少信息输出
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//设置OpenGL版本主版本号 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);//设置OpenGL版本次版本号
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

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        return -1; // 或者其他错误处理方式
    }

    // 设置 OpenGL 调试回调
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // 同步回调
        glDebugMessageCallback(OpenGLDebugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    // 获取 OpenGL 版本信息
    const char* version = (const char*)glGetString(GL_VERSION);
    // 输出 OpenGL 版本
    std::cout << "OpenGL Version: " << version << std::endl;

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
    ImGuiIO& io = ImGui::GetIO(); // 获取 io 对象

    // 添加默认字体
    io.Fonts->AddFontDefault();

    // 设置 ImGui 样式
    ImGui::StyleColorsDark();

    // 初始化平台/渲染绑定
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // 确保根据你的 OpenGL 版本修改
    ImGui::GetIO().FontGlobalScale = 1.5f; // 将字体放大到原来的1.5
    RealTimeRender(window);
}
static void LoadModel(SceneManager& g_SceneManager) {
    g_TextureArray = new TextureArray(1024, 1024, 64);
    //MeshComponent* meshComponent1 = resourceManager.LoadOBJ("res/Obj/OBJ_2247/", "OBJ_2247.obj", 0.3f);
    MeshComponent* meshComponent1 = resourceManager.LoadOBJ("res/Obj/OBJ_2269/", "OBJ_2269.obj", 0.3f);
    //MeshComponent* meshComponent3 = resourceManager.LoadOBJ("res/Obj/RAN Halloween Pumpkin 2024 - OBJ/", "RAN_Halloween_Pumpkin_2024_High_Poly.obj", 10.3f);
    //MeshComponent* meshComponent1 = resourceManager.LoadOBJ("res/Obj/RAN Halloween Pumpkin 2024 - OBJ/", "RAN_Halloween_Pumpkin_2024_High_Poly.obj", 10.3f);
    //MeshComponent* meshComponent1 = resourceManager.LoadOBJ("res/Obj/9130.哥特王座/", "哥特王座.obj", 0.03f);
    g_SceneManager.AddEntity(meshComponent1, "tree", "node1", nullptr);
    PointLight* pointLight = new PointLight("PointLight", _WHITE, 2.288, glm::vec3(0.294f, 0.264f, 3.023f));
    g_SceneManager.AddPointLight(pointLight, "node2", nullptr);
    //g_SceneManager.AddEntity(meshComponent2, "Pumpkin1", "node4", nullptr);
    //g_SceneManager.AddEntity(meshComponent3, "Pumpkin2", "node3", nullptr);
}
static void InitModel() {

}

static void InitCamera(Camera& camera) {
    camera.SetPosition(glm::vec3(25.293f, 2.000f, 4.469f));
    camera.SetTarget(glm::vec3(20.102, 1.561, 3.356));
}
void RealTimeRender(GLFWwindow* window) {
    // 定义视口宽高
    float width = WINDOW_WIDTH;
    float height = WINDOW_HEIGHT;
    float aspect_ratio = width / height;


    // 定义视野角度（以弧度为单位）、近平面和远平面
    float fov = 30.0f; // 30度视野角
    float near_plane = NEAR_PLANE;
    float far_plane = FAR_PLANE;

    Camera camera(fov, aspect_ratio, near_plane, far_plane);
    InitCamera(camera);
    cameraController = new CameraController(&camera, window);

    Scene* scene = new Scene(50);
    g_SceneManager = SceneManager(scene);
    LoadModel(g_SceneManager);
    InitModel();
    scene->ResetVAO();

    int sampleRate = 0;
    Shader* mainShader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/Batch.glsl");
    Shader* depthShader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/depth_shader.glsl");
    Shader* cubeDepthShader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/cubeMapDepth.glsl");
    ColorFBO colorFBO(WINDOW_WIDTH, WINDOW_HEIGHT);
    //这段真的非常非常重要，忘记绑定了。
    //sampler2D是一个unsigned int类型，值对应到Texture的slot 来自凌晨5：31的一条注释
    mainShader->Bind();
    mainShader->SetUniform1f("farPlane", far_plane);
    mainShader->SetUniform1i("ShadowMap", 31);
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
    // 创建天空盒实例
    Skybox skybox(faces);
    DepthMapFBO depthMapFBO(WINDOW_WIDTH, WINDOW_HEIGHT);
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
        cameraController->OnImGuiRender();
        cameraController->Update(deltaTime);
        mainShader->Bind();
        mainShader->SetUniform1i("numPointLights", g_PointLightID.size());
        mainShader->SetUniform1f("focusDepth", camera.GetFocusDepth());
        mainShader->SetUniform1f("focusRange", camera.GetFocusRange());
        mainShader->Unbind();

        ViewPortInit(SHADOW_WIDTH, SHADOW_HEIGHT);
        scene->RenderShadowMap(depthShader, cubeDepthShader);

        //render
        ViewPortInit(WINDOW_WIDTH, WINDOW_HEIGHT);
        colorFBO.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.Draw(camera);
        mainShader->Bind();
        mainShader->SetUniform1i("ShadowMap", 31);
        for (int i = 0; i < 4; i++)
        {
            mainShader->SetUniform1i("PointShadowMap[" + std::to_string(i) + "]", 27 + i);
        }
        scene->BatchRender(*mainShader, camera);
        mainShader->Unbind();
        colorFBO.Unbind();
        //post render
        ColorFBO t = PostRender(colorFBO, camera);
        RenderFBOtoScreen(t);
        //update
        scene->Update(deltaTime);

        ImGui::Begin("RayTracing");
        ImGui::SliderInt("Sample Rate", &sampleRate, 0, 50);
        if (ImGui::Button("Render")) {
            RayTracing(camera, scene, sampleRate, window);
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

void RayTracing(Camera& camera, Scene* scene, int sampleRate, GLFWwindow* window) {
     Shader* mainShader = resourceManager.Load<Shader>("res/shaders/RayTracing/main.glsl");
     Shader* depthShader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/depth_shader.glsl");

     DepthMapFBO depthMapFBO(WINDOW_WIDTH, WINDOW_HEIGHT);

     std::cout << "Depth Rendering..." << std::endl;
     {
         depthShader->Bind();
         depthMapFBO.Bind();
         glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
         depthShader->SetUniformMat4f("SpaceMatrix", camera.GetProjectionMatrix() * camera.GetViewMatrix());
         scene->RenderDepthMap(*depthShader);
         depthMapFBO.Unbind();
         depthShader->Unbind();
     }
     std::cout << "Depth Render Complete!" << std::endl;

     scene->FreeVAO();
     std::cout << "VAO freed!" << std::endl;

     std::cout << "Material SSBO Initializing..." << std::endl;
     ShaderStorageBuffer materialSSBO(g_MaterialList.data(), g_MaterialList.size() * sizeof(Material), 0);
     int numMaterials = g_MaterialList.size();
     std::cout << "Material SSBO Created!" << std::endl;

     ColorFBO colorFBO(WINDOW_WIDTH, WINDOW_HEIGHT);
     std::vector<Triangle> triangles;
     int size = scene->GetIndices()->size();
     std::vector<unsigned int>* indices = scene->GetIndices();
     std::vector<Vertex>* vertices = scene->GetVertices();
     std::cout << "Getting Triangles" << std::endl;
     {
         TriangleSubdivider triangleSubdivider;
         for (int i = 0;i < size - 2; i += 3) {
             unsigned int idx1 = indices->at(i);
             unsigned int idx2 = indices->at(i + 1);
             unsigned int idx3 = indices->at(i + 2);
             Vertex v1 = vertices->at(idx1);
             Vertex v2 = vertices->at(idx2);
             Vertex v3 = vertices->at(idx3);
             // 创建三角形
             Triangle triangle = triangleSubdivider.CreateTriangle(v1, v2, v3);
             triangles.push_back(triangle);
         }
     }
     ShaderStorageBuffer trianglesSSBO(triangles.data(), triangles.size() * sizeof(Triangle), 1);
     int numTriangles = triangles.size();
     std::cout << "numTriangles: " << numTriangles << std::endl;


     std::cout << "Creating BVH Tree..." << std::endl;
     BVHTree BVHTree(triangles);
     std::cout << "BVH Tree Created" << std::endl;

     ShaderStorageBuffer BVHssbo(BVHTree.nodes.data(), BVHTree.nodes.size() * sizeof(BVHNode), 2);
     ShaderStorageBuffer triangleIndexBVHSSBO(BVHTree.triangleIndices.data(), BVHTree.triangleIndices.size() * sizeof(int), 3);

     
     GLint64 maxSSBOSize = 0;
     //错误检查
     {
         glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSSBOSize);
         std::cout << "Maximum SSBO size: " << maxSSBOSize << " bytes" << std::endl;

         size_t ssboSize = triangles.size() * sizeof(Triangle);
         std::cout << "Current SSBO size: " << ssboSize << " bytes" << std::endl;

         if (ssboSize > maxSSBOSize) {
             std::cerr << "Error: SSBO size exceeds the maximum allowed size." << std::endl;
             // 采取措施，例如减少场景复杂度或分批处理
             return;
         }
     }

     mainShader->Bind();
     depthMapFBO.BindTexture(0);
     mainShader->SetUniform1i("depthMap", 0);
     mainShader->SetUniform1i("numTriangles", numTriangles);
     mainShader->SetUniform1i("numMaterials", numMaterials);
     mainShader->Unbind();

     std::vector<cv::Mat> matArray;
     for (int i = 0; i <= sampleRate; i++) {
         std::cout << "Render Mat " << i << std::endl;
         //Bind ssbo
         trianglesSSBO.Bind();
         BVHssbo.Bind();
         triangleIndexBVHSSBO.Bind();
         //render
         colorFBO.Bind();
         GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
         scene->RayTracingRender(*mainShader, camera, window);
         colorFBO.Unbind();
         ColorFBO postRenderFBO = PostRender(colorFBO, depthMapFBO, camera);
         cv::Mat mat = resourceManager.SaveFBOToMat(postRenderFBO, WINDOW_WIDTH, WINDOW_HEIGHT);
         // 确保图像是4通道的
         if (mat.channels() == 1) {
             cv::cvtColor(mat, mat, cv::COLOR_GRAY2BGRA);
         }
         else if (mat.channels() == 3) {
             cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);
         }
         else if (mat.channels() == 4) {
             std::cout << "mat" << i << " channels: " << mat.channels() << std::endl;
         }
         else {
             std::cerr << "Unsupported number of channels: " << mat.channels() << ". Skipping this image." << std::endl;
             continue;
         }
         matArray.push_back(mat);
         BVHssbo.Unbind();
         triangleIndexBVHSSBO.Unbind();
         trianglesSSBO.Unbind();
         std::cout << "Render Completed" << std::endl;
     }

     cv::Mat denoisedPicture;
     // 去噪处理：通过平均多个图像
     if (!matArray.empty()) {
         // 获取第一张图像的尺寸和通道数
         cv::Size referenceSize = matArray[0].size();
         int referenceChannels = matArray[0].channels();

         // 初始化累加器为浮点型，避免精度丢失
         cv::Mat accumulator = cv::Mat::zeros(referenceSize, CV_32FC4); // 修改为4通道

         for (size_t i = 0; i < matArray.size(); ++i) {
             const cv::Mat& mat = matArray[i];
             std::cout << "Image " << i << ": size = " << mat.size()
                 << ", channels = " << mat.channels()
                 << ", type = " << mat.type()
                 << ", isContinuous = " << mat.isContinuous()
                 << ", empty = " << mat.empty() << std::endl;
             // 检查图像是否为空
             if (mat.empty()) {
                 std::cerr << "Error: Image " << i << " is empty. Skipping this image." << std::endl;
                 continue;
             }

             // 检查尺寸
             if (mat.size() != referenceSize) {
                 std::cerr << "Error: Image " << i << " size mismatch. Expected " << referenceSize << ", got " << mat.size() << ". Skipping this image." << std::endl;
                 continue;
             }

             // 检查通道数
             if (mat.channels() != 4) {
                 std::cerr << "Error: Image " << i << " channel mismatch. Expected 4 channels, got " << mat.channels() << ". Skipping this image." << std::endl;
                 continue;
             }

             // 转换为32位浮点数
             cv::Mat matFloat = cv::Mat::zeros(referenceSize, CV_32FC4);
             mat.convertTo(matFloat, CV_32FC4); // 修改为4通道

             // 累加
             try {
                 accumulator += matFloat;
             }
             catch (const cv::Exception& e) {
                 std::cerr << "OpenCV Exception during accumulation of Image " << i << ": " << e.what() << std::endl;
                 continue;
             }
         }

         // 计算平均值
         accumulator /= static_cast<float>(matArray.size());

         // 转换回8位图像
         accumulator.convertTo(denoisedPicture, CV_8UC4);

         std::cout << "Denoising completed by averaging." << std::endl;
     }
     else {
         // 处理 matArray 为空的情况
         std::cerr << "Error: matArray is empty. No images to denoise." << std::endl;
     }

     cv::Mat finalPicture = denoisedPicture;

     // 保存去噪后的图像
     resourceManager.SaveMatToPNG(finalPicture, "final.png", WINDOW_WIDTH, WINDOW_HEIGHT);
     std::cout << "Final image saved as final.png" << std::endl;

     scene->ResetVAO();
     std::cout << "VAO reset!" << std::endl;

}
static void RenderFBOtoScreen(ColorFBO& colorFBO) {
    Quad screenQuad;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader* screenShader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/screen.glsl");
    colorFBO.BindTexture(0);
    colorFBO.BindDepthTexture(1);
    screenShader->Bind();
    screenShader->SetUniform1i("screenTexture", 0);
    //screenShader->SetUniform1i("depthTexture", 1);
    screenQuad.Render(*screenShader);
    screenShader->Unbind();
    delete screenShader;
}


static ColorFBO PostRender(ColorFBO& colorFBO, Camera& camera) {
    static Quad screenQuad;
    ColorFBO finalFBO(WINDOW_WIDTH, WINDOW_HEIGHT);

    Shader* FODshader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/FOD.glsl");
    finalFBO.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        colorFBO.BindTexture(0);
        colorFBO.BindDepthTexture(1);
        FODshader->Bind();
            FODshader->SetUniform1i("screenTexture", 0);
            FODshader->SetUniform1i("depthTexture", 1);
            FODshader->SetUniform1f("focusDepth", camera.GetFocusDepth());
            FODshader->SetUniform1f("focusRange", camera.GetFocusRange());
            FODshader->SetUniform1f("maxBlur", camera.GetMaxBlur());
            screenQuad.Render(*FODshader);
        FODshader->Unbind();
    finalFBO.Unbind();
    delete FODshader;
    return finalFBO;
}

static ColorFBO PostRender(ColorFBO& colorFBO, DepthMapFBO& depthFBO, Camera& camera) {
    static Quad screenQuad;
    ColorFBO finalFBO(WINDOW_WIDTH, WINDOW_HEIGHT);

    Shader* FODshader = resourceManager.Load<Shader>("res/shaders/RayTracing/FOD.glsl");
    finalFBO.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    colorFBO.BindTexture(0);
    depthFBO.BindTexture(1);
    FODshader->Bind();
    FODshader->SetUniform1i("screenTexture", 0);
    FODshader->SetUniform1i("depthTexture", 1);
    FODshader->SetUniform1f("focusDepth", camera.GetFocusDepth());
    FODshader->SetUniform1f("focusRange", camera.GetFocusRange());
    FODshader->SetUniform1f("maxBlur", camera.GetMaxBlur());
    screenQuad.Render(*FODshader);
    FODshader->Unbind();
    finalFBO.Unbind();
    delete FODshader;
    return finalFBO;
}