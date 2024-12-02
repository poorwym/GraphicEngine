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
#include <map>

#include "opencv2/opencv.hpp"
#include "ShaderStorageBuffer.h"
#include "Filter.h"
#include "BVHTree.h"

DirectionalLightController directionalLightController;

float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

// �������������Դ������
CameraController* cameraController = nullptr;
ResourceManager resourceManager;
SceneManager sceneManager(nullptr);

void RayTracing(Camera& camera, Scene* scene);
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
    // ���� OpenCV ��־����Ϊ ERROR��������Ϣ���
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//����OpenGL�汾���汾�� 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);//����OpenGL�汾�ΰ汾��
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

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        return -1; // ��������������ʽ
    }

    // ��ȡ OpenGL �汾��Ϣ
    const char* version = (const char*)glGetString(GL_VERSION);
    // ��� OpenGL �汾
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
    textureArray = new TextureArray(1024, 1024, 256);
    MeshComponent* meshComponent1 = resourceManager.LoadOBJ("res/Obj/OBJ_2247/", "OBJ_2247.obj", 0.3f);
    sceneManager.AddEntity(meshComponent1, "Pumpkin", "node1", nullptr);
    PointLight* pointLight = new PointLight("PointLight", _WHITE, 2.288, glm::vec3(0.294f, 0.264f, 3.023f));
    sceneManager.AddPointLight(pointLight, "node2", nullptr);
}
static void InitModel() {

}

static void InitCamera(Camera& camera) {
    camera.SetPosition(glm::vec3(25.293f, 2.000f, 4.469f));
    camera.SetTarget(glm::vec3(20.102, 1.561, 3.356));
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
    scene->ResetVAO();

    Shader* mainShader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/Batch.shader");
    Shader* depthShader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/depth_shader.shader");
    Shader* cubeDepthShader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/cubeMapDepth.shader");
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
        scene->BatchRender(*mainShader, camera);
        mainShader->Unbind();
        colorFBO.Unbind();
        //post render
        ColorFBO t = PostRender(colorFBO, camera);
        RenderFBOtoScreen(t);
        //update
        scene->Update(deltaTime);
        

        ImGui::Begin("RayTracing");
        if (ImGui::Button("Render")) {
            RayTracing(camera, scene);
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

void RayTracing(Camera& camera, Scene* scene) {
     Shader* mainShader = resourceManager.Load<Shader>("res/shaders/RayTracing/RayTracing.shader");
     ColorFBO colorFBO(WINDOW_WIDTH, WINDOW_HEIGHT);
     std::vector<Triangle> triangles;
     int size = scene->GetIndices()->size();
     std::vector<unsigned int>* indices = scene->GetIndices();
     std::vector<Vertex>* vertices = scene->GetVertices();
     for (int i = 0;i < size - 2; i += 3) {
         unsigned int idx1 = indices->at(i);
         unsigned int idx2 = indices->at(i + 1);
         unsigned int idx3 = indices->at(i + 2);
         Vertex v1 = vertices->at(idx1);
         Vertex v2 = vertices->at(idx2);
         Vertex v3 = vertices->at(idx3);
         Triangle triangle;
         triangle.position[0] = glm::vec4(v1.Position, 0.0f);
         triangle.position[1] = glm::vec4(v2.Position, 0.0f);
         triangle.position[2] = glm::vec4(v3.Position, 0.0f);
         triangle.normal[0] = glm::vec4(v1.Normal, 0.0f);
         triangle.normal[1] = glm::vec4(v2.Normal, 0.0f);
         triangle.normal[2] = glm::vec4(v3.Normal, 0.0f);
         triangle.texCoords[0] = glm::vec4(v1.TexCoords, 0.0f, 0.0f);
         triangle.texCoords[1] = glm::vec4(v2.TexCoords, 0.0f, 0.0f);
         triangle.texCoords[2] = glm::vec4(v3.TexCoords, 0.0f, 0.0f);
         triangle.tangent[0] = glm::vec4(v1.Tangent, 0.0f);
         triangle.tangent[1] = glm::vec4(v2.Tangent, 0.0f);
         triangle.tangent[2] = glm::vec4(v3.Tangent, 0.0f);
         triangle.bitangent[0] = glm::vec4(v1.Bitangent, 0.0f);
         triangle.bitangent[1] = glm::vec4(v2.Bitangent, 0.0f);
         triangle.bitangent[2] = glm::vec4(v3.Bitangent, 0.0f);
         triangle.material = v1.material;
         triangles.push_back(triangle);
     }
     ShaderStorageBuffer* trianglesSSBO = new ShaderStorageBuffer(triangles.data(), triangles.size() * sizeof(Triangle), 0);
     int numTriangles = triangles.size();
     BVHTree tree(triangles);
     ShaderStorageBuffer* BVHTreeSSBO = new ShaderStorageBuffer(tree.nodes.data(), tree.nodes.size() * sizeof(BVHNode), 1);
     ShaderStorageBuffer* triangleIndexSSBO = new ShaderStorageBuffer(tree.triangleIndices.data(), tree.triangleIndices.size() * sizeof(int), 2);

     GLint64 maxSSBOSize = 0;
     //������
     {
         glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSSBOSize);
         std::cout << "Maximum SSBO size: " << maxSSBOSize << " bytes" << std::endl;

         size_t ssboSize = triangles.size() * sizeof(Triangle);
         std::cout << "Current SSBO size: " << ssboSize << " bytes" << std::endl;

         if (ssboSize > maxSSBOSize) {
             std::cerr << "Error: SSBO size exceeds the maximum allowed size." << std::endl;
             // ��ȡ��ʩ��������ٳ������ӶȻ��������
             return;
         }
     }
     mainShader->Bind();
     mainShader->setUniform1i("numTriangles", numTriangles);
     mainShader->Unbind();

     std::vector<cv::Mat> matArray;
     for (int i = 0; i <= 10; i++) {
         std::cout << "Render Mat " << i << std::endl;
         //Bind ssbo
         trianglesSSBO->Bind();
         BVHTreeSSBO->Bind();
         triangleIndexSSBO->Bind();
         //render
         colorFBO.Bind();
         GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
         scene->RayTracingRender(*mainShader, camera);
         colorFBO.Unbind();
         //ColorFBO postRenderFBO = PostRender(colorFBO, camera);
         cv::Mat mat = resourceManager.SaveFBOToMat(colorFBO, WINDOW_WIDTH, WINDOW_HEIGHT);
         // ȷ��ͼ����4ͨ����
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
         trianglesSSBO->Unbind();
         BVHTreeSSBO->Unbind();
         triangleIndexSSBO->Unbind();
         std::cout << "Render Completed" << std::endl;
     }
     trianglesSSBO->Unbind();

     cv::Mat denoisedPicture;
     // ȥ�봦��ͨ��ƽ�����ͼ��
     if (!matArray.empty()) {
         // ��ȡ��һ��ͼ��ĳߴ��ͨ����
         cv::Size referenceSize = matArray[0].size();
         int referenceChannels = matArray[0].channels();

         // ��ʼ���ۼ���Ϊ�����ͣ����⾫�ȶ�ʧ
         cv::Mat accumulator = cv::Mat::zeros(referenceSize, CV_32FC4); // �޸�Ϊ4ͨ��

         for (size_t i = 0; i < matArray.size(); ++i) {
             const cv::Mat& mat = matArray[i];
             std::cout << "Image " << i << ": size = " << mat.size()
                 << ", channels = " << mat.channels()
                 << ", type = " << mat.type()
                 << ", isContinuous = " << mat.isContinuous()
                 << ", empty = " << mat.empty() << std::endl;
             // ���ͼ���Ƿ�Ϊ��
             if (mat.empty()) {
                 std::cerr << "Error: Image " << i << " is empty. Skipping this image." << std::endl;
                 continue;
             }

             // ���ߴ�
             if (mat.size() != referenceSize) {
                 std::cerr << "Error: Image " << i << " size mismatch. Expected " << referenceSize << ", got " << mat.size() << ". Skipping this image." << std::endl;
                 continue;
             }

             // ���ͨ����
             if (mat.channels() != 4) {
                 std::cerr << "Error: Image " << i << " channel mismatch. Expected 4 channels, got " << mat.channels() << ". Skipping this image." << std::endl;
                 continue;
             }

             // ת��Ϊ32λ������
             cv::Mat matFloat = cv::Mat::zeros(referenceSize, CV_32FC4);
             mat.convertTo(matFloat, CV_32FC4); // �޸�Ϊ4ͨ��

             // �ۼ�
             try {
                 accumulator += matFloat;
             }
             catch (const cv::Exception& e) {
                 std::cerr << "OpenCV Exception during accumulation of Image " << i << ": " << e.what() << std::endl;
                 continue;
             }
         }

         // ����ƽ��ֵ
         accumulator /= static_cast<float>(matArray.size());

         // ת����8λͼ��
         accumulator.convertTo(denoisedPicture, CV_8UC4);

         std::cout << "Denoising completed by averaging." << std::endl;
     }
     else {
         // ���� matArray Ϊ�յ����
         std::cerr << "Error: matArray is empty. No images to denoise." << std::endl;
     }

     cv::Mat finalPicture = denoisedPicture;

     // ����ȥ����ͼ��
     resourceManager.SaveMatToPNG(finalPicture, "final.png", WINDOW_WIDTH, WINDOW_HEIGHT);
     std::cout << "Final image saved as final.png" << std::endl;

     // �ͷ���Դ
     delete trianglesSSBO;
}
static void RenderFBOtoScreen(ColorFBO& colorFBO) {
    Quad screenQuad;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader* screenShader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/screen.shader");
    colorFBO.BindTexture(0);
    colorFBO.BindDepthTexture(1);
    screenShader->Bind();
    screenShader->setUniform1i("screenTexture", 0);
    //screenShader->setUniform1i("depthTexture", 1);
    screenQuad.Render(*screenShader);
    screenShader->Unbind();
    delete screenShader;
}

static ColorFBO PostRender(ColorFBO& colorFBO, Camera& camera) {
    static Quad screenQuad;
    ColorFBO finalFBO(WINDOW_WIDTH, WINDOW_HEIGHT);

    Shader* FODshader = resourceManager.Load<Shader>("res/shaders/RealTimeRendering/FOD.shader");
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
    delete FODshader;
    return finalFBO;
}