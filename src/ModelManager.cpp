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
    // 初始化模型列表和缩放因子
    // 添加之前所有的 OBJ 模型
    AddModel("OBJ", "res/Obj/OBJ_2247/", "OBJ_2247.obj", 0.3f);
    AddModel("OBJ", "res/Obj/OBJ_2269/", "OBJ_2269.obj", 0.3f);
    AddModel("OBJ", "res/Obj/RAN Halloween Pumpkin 2024 - OBJ/", "RAN_Halloween_Pumpkin_2024_High_Poly.obj", 10.3f);
    AddModel("OBJ", "res/Obj/9130.哥特王座/", "哥特王座.obj", 0.03f);
    AddModel("OBJ", "res/Obj/bust/", "bust.obj", 0.3f);
    AddModel("OBJ", "res/Obj/Fairy/", "WildFantasyFairy.obj", 50.0f);
    AddModel("OBJ", "res/Obj/gandam/", "1.obj", 0.5f);

    // 添加之前所有的 GLTF 模型
    AddModel("GLTF", "res/gltf/kitchen/", "Come Celebrate Thanksgiving With Invrsion!.gltf", 1.0f);
    AddModel("GLTF", "res/gltf/Fairy/", "Wild Fantasy Fairy.gltf", 20.0f);
}

ModelManager::~ModelManager()
{
    // 注意：如果 m_Scene 不应由 ModelManager 管理，避免在此处删除
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

    // OBJ 模型部分
    ImGui::Text("OBJ Models");
    for (const auto& model : m_OBJModelList)
    {
        if (ImGui::Button(model.second.c_str()))
        {
            Load(model.first, model.second, "OBJ");
        }
    }

    // GLTF 模型部分
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
        // 获取缩放因子
        float scale = 1.0f;
        auto it = m_ModelScale.find(fileName);
        if (it != m_ModelScale.end()) {
            scale = it->second;
        }

        // 加载 OBJ 模型
        MeshComponent* meshComponent = m_ResourceManager.LoadOBJ(filePath, fileName, scale);
        if (meshComponent) {
            // 生成一个唯一的实体名称，可以根据需要调整
            std::string entityName = fileName.substr(0, fileName.find_last_of('.'));
            // 添加到 SceneManager
            m_SceneManager.AddEntity(meshComponent, entityName.c_str(), (entityName + "_node" + std::to_string(counter++)).c_str(), nullptr);
            std::cout << "Loaded OBJ model: " << fileName << std::endl;
        }
        else {
            std::cerr << "Failed to load OBJ model: " << fileName << std::endl;
        }
    }
    else if (type == "GLTF") {
        // 获取缩放因子
        float scale = 1.0f;
        auto it = m_ModelScale.find(fileName);
        if (it != m_ModelScale.end()) {
            scale = it->second;
        }

        // 加载 GLTF 模型
        SceneNode* node = m_ResourceManager.LoadGLTF(filePath, fileName, scale);
        if (node) {
            // 生成一个唯一的节点名称，可以根据需要调整
            std::string nodeName = fileName.substr(0, fileName.find_last_of('.'));
            // 添加到 SceneManager
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