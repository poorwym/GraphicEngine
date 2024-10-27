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


float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

// �������������Դ������
CameraController* cameraController = nullptr;
ResourceManager resourceManager;

void test2D(GLFWwindow* window);
void test3D(GLFWwindow* window);

// ����ƶ��ص�����
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (cameraController)
        cameraController->ProcessMouseInput(static_cast<float>(xpos), static_cast<float>(ypos));
}

// �����ֻص�����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (cameraController)
        cameraController->ProcessMouseScroll(static_cast<float>(yoffset));
}


std::vector<Vertex> vertices = {
    // ǰ��
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // ǰ����
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // ǰ����
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // ǰ����
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // ǰ����

    // ����
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // ������
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // ������
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // ������
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // ������

    // ����
    {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // ������
    {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // ǰ����
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // ������
    {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // ǰ����

    // ����
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // ������
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // ǰ����
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // ������
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // ǰ����

    // ����
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // ������
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, // ������
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // ǰ����
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // ǰ����

    // ����
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // ������
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // ������
    {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // ǰ����
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // ǰ����
};

// �����������
std::vector<unsigned int> indices = {
    // ǰ��
    0, 1, 2,
    2, 1, 3,

    // ����
    4, 5, 6,
    6, 5, 7,

    // ����
    8, 9, 10,
    10, 9, 11,

    // ����
    12, 13, 14,
    14, 13, 15,

    // ����
    16, 17, 18,
    18, 17, 19,

    // ����
    20, 21, 22,
    22, 21, 23,
};

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
    ImGui::GetIO().FontGlobalScale = 1.5f; // ������Ŵ�ԭ����1.5��

    

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

    // �����ӿڿ��
    float width = 1920.0f;
    float height = 1080.0f;
    float aspect_ratio = width / height;

    // ������Ұ�Ƕȣ��Ի���Ϊ��λ������ƽ���Զƽ��
    float fov = 45.0f; // 45����Ұ��
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