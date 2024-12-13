/*
#include "FBXManager.h"
#include "ResourceManager.h"
#include <iostream>

// 构造函数
FBXManager::FBXManager()
    : m_Manager(nullptr), m_Scene(nullptr)
{
    InitializeSdkObjects(m_Manager, m_Scene);
}

// 析构函数
FBXManager::~FBXManager()
{
    if (m_Scene) m_Manager->DestroyScene(m_Scene);
    if (m_Manager) m_Manager->Destroy();
}

// 初始化FBX SDK对象
void FBXManager::InitializeSdkObjects(FbxManager*& manager, FbxScene*& scene)
{
    // 创建FBX Manager
    manager = FbxManager::Create();
    if (!manager) {
        std::cerr << "Unable to create FBX Manager!\n";
        return;
    }

    // 创建IO设置
    FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
    manager->SetIOSettings(ios);

    // 创建FBX场景
    scene = FbxScene::Create(manager, "FBX_Scene");
    if (!scene) {
        std::cerr << "Unable to create FBX Scene!\n";
        return;
    }
}

// 加载FBX场景
bool FBXManager::LoadScene(const std::string& filePath, const std::string& fileName)
{
    // 创建导入器
    FbxImporter* importer = FbxImporter::Create(m_Manager, "");

    std::string fullPath = filePath + fileName;

    // 初始化导入器
    if (!importer->Initialize(fullPath.c_str(), -1, m_Manager->GetIOSettings())) {
        std::cerr << "FBX Importer initialization failed: " << importer->GetStatus().GetErrorString() << std::endl;
        importer->Destroy();
        return false;
    }

    // 导入场景
    if (!importer->Import(m_Scene)) {
        std::cerr << "FBX Import failed: " << importer->GetStatus().GetErrorString() << std::endl;
        importer->Destroy();
        return false;
    }

    importer->Destroy();
    return true;
}

// 加载FBX文件并返回根节点
SceneNode* FBXManager::Load(const std::string& filePath, const std::string& fileName)
{
    if (!LoadScene(filePath, fileName)) {
        return nullptr;
    }

    // 获取FBX场景的根节点
    FbxNode* fbxRootNode = m_Scene->GetRootNode();
    if (!fbxRootNode) {
        std::cerr << "FBX Scene has no root node.\n";
        return nullptr;
    }

    // 创建根SceneNode
    SceneNode* rootNode = new SceneNode("Root", static_cast<void*>(nullptr), nullptr); // 空节点

    // 递归处理FBX节点
    for (int i = 0; i < fbxRootNode->GetChildCount(); ++i) {
        FbxNode* child = fbxRootNode->GetChild(i);
        ProcessFbxNode(child, rootNode);
    }

    return rootNode;
}

// 递归处理FBX节点并创建SceneNode
SceneNode* FBXManager::ProcessFbxNode(FbxNode* fbxNode, SceneNode* parentNode)
{
    std::string nodeName = fbxNode->GetName();
    glm::mat4 localTransform = ConvertFbxTransform(fbxNode);

    // 检查节点是否包含网格
    FbxMesh* fbxMesh = fbxNode->GetMesh();
    Entity* entity = nullptr;
    MeshComponent* meshComponent = nullptr;
    if (fbxMesh) {
        entity = new Entity(nodeName);
        meshComponent = ResourceManager::LoadFBXMesh(fbxMesh, m_Scene->GetName()); // 需要实现LoadFBXMesh
        if (meshComponent) {
            entity->AddComponent(meshComponent);
        }
    }

    // 检查节点是否是光源
    Light* light = nullptr;
    ProcessLight(fbxNode, &light);

    // 创建SceneNode
    SceneNode* sceneNode = nullptr;
    if (entity) {
        sceneNode = new SceneNode(nodeName, entity, parentNode);
    }
    else if (light) {
        // 根据光源类型，创建相应的SceneNode构造函数
        // 这里只处理点光源作为示例，你可以扩展处理其他光源类型
        if (light->GetType() == "PointLight") {
            PointLight* pointLight = dynamic_cast<PointLight*>(light);
            sceneNode = new SceneNode(nodeName, pointLight, parentNode);
        }
        else if (light->GetType() == "DirectionalLight") {
            DirectionalLight* directionalLight = dynamic_cast<DirectionalLight*>(light);
            sceneNode = new SceneNode(nodeName, directionalLight, parentNode);
        }
        else if (light->GetType() == "SpotLight") {
            SpotLight* spotLight = dynamic_cast<SpotLight*>(light);
            sceneNode = new SceneNode(nodeName, spotLight, parentNode);
        }
    }
    else {
        // 如果节点既不是网格也不是光源，可以创建一个空的SceneNode
        sceneNode = new SceneNode(nodeName, static_cast<void*>(nullptr), parentNode);
    }

    // 递归处理子节点
    for (int i = 0; i < fbxNode->GetChildCount(); ++i) {
        FbxNode* child = fbxNode->GetChild(i);
        ProcessFbxNode(child, sceneNode);
    }

    return sceneNode;
}

// 处理网格并返回MeshComponent
void FBXManager::ProcessMesh(FbxMesh* fbxMesh, Entity* entity)
{
    MeshComponent* meshComponent = ResourceManager::LoadFBXMesh(fbxMesh, m_Scene->GetName());
    if (meshComponent) {
        entity->AddComponent(meshComponent);
    }
}

// 处理材质并返回PBRMaterial
PBRMaterial* FBXManager::ProcessMaterial(FbxSurfaceMaterial* fbxMaterial, const std::string& filePath)
{
    // 转换FBX材质到PBRMaterial
    PBRMaterial* pbrMaterial = nullptr;

    pbrMaterial = ResourceManager::LoadPBRMaterial(fbxMaterial, filePath);

    return pbrMaterial;
}

// 处理光源并返回Light对象
void FBXManager::ProcessLight(FbxNode* fbxNode, Light** light)
{
    FbxLight* fbxLight = fbxNode->GetLight();
    if (!fbxLight) {
        *light = nullptr;
        return;
    }

    // 获取光源类型
    FbxLight::EType lightType = fbxLight->LightType.Get();

    switch (lightType)
    {
    case FbxLight::ePoint:
    {
        // 创建PointLight
        glm::vec3 color(
            static_cast<float>(fbxLight->Color.Get()[0]),
            static_cast<float>(fbxLight->Color.Get()[1]),
            static_cast<float>(fbxLight->Color.Get()[2])
        );
        float intensity = static_cast<float>(fbxLight->Intensity.Get());
        glm::vec3 position = glm::vec3(0.0f); // 位置将在SceneNode的变换中设置

        *light = new PointLight("PointLight_" + std::string(fbxNode->GetName()), color, intensity, position);
        break;
    }
    case FbxLight::eDirectional:
    {
        // 创建DirectionalLight
        glm::vec3 color(
            static_cast<float>(fbxLight->Color.Get()[0]),
            static_cast<float>(fbxLight->Color.Get()[1]),
            static_cast<float>(fbxLight->Color.Get()[2])
        );
        float intensity = static_cast<float>(fbxLight->Intensity.Get());
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f); // 方向将在SceneNode的变换中设置

        *light = new DirectionalLight("DirectionalLight_" + std::string(fbxNode->GetName()), color, intensity, direction);
        break;
    }
    case FbxLight::eSpot:
    {
        // 创建SpotLight
        glm::vec3 color(
            static_cast<float>(fbxLight->Color.Get()[0]),
            static_cast<float>(fbxLight->Color.Get()[1]),
            static_cast<float>(fbxLight->Color.Get()[2])
        );
        float intensity = static_cast<float>(fbxLight->Intensity.Get());
        glm::vec3 position = glm::vec3(0.0f); // 位置将在SceneNode的变换中设置
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f); // 方向将在SceneNode的变换中设置
        float innerConeAngle = static_cast<float>(fbxLight->InnerAngle.Get());
        float outerConeAngle = static_cast<float>(fbxLight->OuterAngle.Get());

        *light = new SpotLight("SpotLight_" + std::string(fbxNode->GetName()), color, intensity, position, direction, innerConeAngle, outerConeAngle);
        break;
    }
    default:
        *light = nullptr;
        break;
    }
}

// 转换FBX节点的变换到glm::mat4
glm::mat4 FBXManager::ConvertFbxTransform(FbxNode* fbxNode)
{
    FbxAMatrix fbxMatrix = fbxNode->EvaluateGlobalTransform();

    // FBX SDK使用的是右手坐标系，GLM默认是右手坐标系，可能需要转换轴
    // 这里只是一个简单的转换示例，具体转换取决于你的引擎需求
    glm::mat4 glmMatrix = glm::mat4(
        static_cast<float>(fbxMatrix.Get(0, 0)), static_cast<float>(fbxMatrix.Get(0, 1)), static_cast<float>(fbxMatrix.Get(0, 2)), static_cast<float>(fbxMatrix.Get(0, 3)),
        static_cast<float>(fbxMatrix.Get(1, 0)), static_cast<float>(fbxMatrix.Get(1, 1)), static_cast<float>(fbxMatrix.Get(1, 2)), static_cast<float>(fbxMatrix.Get(1, 3)),
        static_cast<float>(fbxMatrix.Get(2, 0)), static_cast<float>(fbxMatrix.Get(2, 1)), static_cast<float>(fbxMatrix.Get(2, 2)), static_cast<float>(fbxMatrix.Get(2, 3)),
        static_cast<float>(fbxMatrix.Get(3, 0)), static_cast<float>(fbxMatrix.Get(3, 1)), static_cast<float>(fbxMatrix.Get(3, 2)), static_cast<float>(fbxMatrix.Get(3, 3))
    );

    return glmMatrix;
}
*/