#include "ModelManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include "EngineState.h"
extern EngineState g_EngineState;

ModelManager::ModelManager(Scene* scene)
    : m_Scene(scene), m_SceneManager(scene)
{
    // ��ʼ��ģ���б����������
    // ���֮ǰ���е� OBJ ģ��
    AddModel("OBJ", "res/Obj/OBJ_2247/", "OBJ_2247.obj", 0.3f);
    AddModel("OBJ", "res/Obj/OBJ_2269/", "OBJ_2269.obj", 0.3f);
    AddModel("OBJ", "res/Obj/RAN Halloween Pumpkin 2024 - OBJ/", "RAN_Halloween_Pumpkin_2024_High_Poly.obj", 10.3f);
    AddModel("OBJ", "res/Obj/9130.��������/", "��������.obj", 0.03f);
    AddModel("OBJ", "res/Obj/bust/", "bust.obj", 0.3f);
    AddModel("OBJ", "res/Obj/Fairy/", "WildFantasyFairy.obj", 50.0f);
    AddModel("OBJ", "res/Obj/gandam/", "1.obj", 0.5f);

    // ���֮ǰ���е� GLTF ģ��
    AddModel("GLTF", "res/gltf/kitchen/", "Come Celebrate Thanksgiving With Invrsion!.gltf", 1.0f);
    AddModel("GLTF", "res/gltf/Fairy/", "Wild Fantasy Fairy.gltf", 20.0f);
}

ModelManager::~ModelManager()
{
    // ע�⣺��� m_Scene ��Ӧ�� ModelManager ���������ڴ˴�ɾ��
    // delete m_Scene;
}

void ModelManager::AddModel(const std::string& type, const std::string& filePath, const std::string& fileName, float scale)
{
    if (type == "OBJ") {
        m_OBJModelList[filePath] = fileName;
    }
    else if (type == "GLTF") {
        m_GLTFModelList[filePath] = fileName;
    }
    m_ModelScale[fileName] = scale;
}

void ModelManager::OnImGuiRender()
{
    ImGui::Begin("Model Manager");

    // OBJ ģ�Ͳ���
    ImGui::Text("OBJ Models");
    for (const auto& model : m_OBJModelList)
    {
        if (ImGui::Button(model.second.c_str()))
        {
            Load(model.first, model.second, "OBJ");
        }
    }

    // GLTF ģ�Ͳ���
    ImGui::Text("GLTF Models");
    for (const auto& model : m_GLTFModelList)
    {
        if (ImGui::Button(model.second.c_str()))
        {
            Load(model.first, model.second, "GLTF");
        }
    }

    ImGui::End();
}

void ModelManager::Load(const std::string& filePath, const std::string& fileName, const std::string& type)
{
    static int counter = 0;
    if (type == "OBJ") {
        // ��ȡ��������
        float scale = 1.0f;
        auto it = m_ModelScale.find(fileName);
        if (it != m_ModelScale.end()) {
            scale = it->second;
        }

        // ���� OBJ ģ��
        MeshComponent* meshComponent = m_ResourceManager.LoadOBJ(filePath, fileName, scale);
        if (meshComponent) {
            // ����һ��Ψһ��ʵ�����ƣ����Ը�����Ҫ����
            std::string entityName = fileName.substr(0, fileName.find_last_of('.'));
            // ��ӵ� SceneManager
            m_SceneManager.AddEntity(meshComponent, entityName.c_str(), (entityName + "_node" + std::to_string(counter++)).c_str(), nullptr);
            std::cout << "Loaded OBJ model: " << fileName << std::endl;
        }
        else {
            std::cerr << "Failed to load OBJ model: " << fileName << std::endl;
        }
    }
    else if (type == "GLTF") {
        // ��ȡ��������
        float scale = 1.0f;
        auto it = m_ModelScale.find(fileName);
        if (it != m_ModelScale.end()) {
            scale = it->second;
        }

        // ���� GLTF ģ��
        SceneNode* node = m_ResourceManager.LoadGLTF(filePath, fileName, scale);
        if (node) {
            // ����һ��Ψһ�Ľڵ����ƣ����Ը�����Ҫ����
            std::string nodeName = fileName.substr(0, fileName.find_last_of('.'));
            // ��ӵ� SceneManager
            m_SceneManager.AddSceneNode(node, nullptr);
            std::cout << "Loaded GLTF model: " << fileName << std::endl;
        }
        else {
            std::cerr << "Failed to load GLTF model: " << fileName << std::endl;
        }
    }
    else {
        std::cerr << "Unknown model type: " << type << std::endl;
    }
    g_EngineState.needUpdate = true;
    m_Scene->ResetVAO();
    
}