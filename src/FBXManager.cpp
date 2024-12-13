/*
#include "FBXManager.h"
#include "ResourceManager.h"
#include <iostream>

// ���캯��
FBXManager::FBXManager()
    : m_Manager(nullptr), m_Scene(nullptr)
{
    InitializeSdkObjects(m_Manager, m_Scene);
}

// ��������
FBXManager::~FBXManager()
{
    if (m_Scene) m_Manager->DestroyScene(m_Scene);
    if (m_Manager) m_Manager->Destroy();
}

// ��ʼ��FBX SDK����
void FBXManager::InitializeSdkObjects(FbxManager*& manager, FbxScene*& scene)
{
    // ����FBX Manager
    manager = FbxManager::Create();
    if (!manager) {
        std::cerr << "Unable to create FBX Manager!\n";
        return;
    }

    // ����IO����
    FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
    manager->SetIOSettings(ios);

    // ����FBX����
    scene = FbxScene::Create(manager, "FBX_Scene");
    if (!scene) {
        std::cerr << "Unable to create FBX Scene!\n";
        return;
    }
}

// ����FBX����
bool FBXManager::LoadScene(const std::string& filePath, const std::string& fileName)
{
    // ����������
    FbxImporter* importer = FbxImporter::Create(m_Manager, "");

    std::string fullPath = filePath + fileName;

    // ��ʼ��������
    if (!importer->Initialize(fullPath.c_str(), -1, m_Manager->GetIOSettings())) {
        std::cerr << "FBX Importer initialization failed: " << importer->GetStatus().GetErrorString() << std::endl;
        importer->Destroy();
        return false;
    }

    // ���볡��
    if (!importer->Import(m_Scene)) {
        std::cerr << "FBX Import failed: " << importer->GetStatus().GetErrorString() << std::endl;
        importer->Destroy();
        return false;
    }

    importer->Destroy();
    return true;
}

// ����FBX�ļ������ظ��ڵ�
SceneNode* FBXManager::Load(const std::string& filePath, const std::string& fileName)
{
    if (!LoadScene(filePath, fileName)) {
        return nullptr;
    }

    // ��ȡFBX�����ĸ��ڵ�
    FbxNode* fbxRootNode = m_Scene->GetRootNode();
    if (!fbxRootNode) {
        std::cerr << "FBX Scene has no root node.\n";
        return nullptr;
    }

    // ������SceneNode
    SceneNode* rootNode = new SceneNode("Root", static_cast<void*>(nullptr), nullptr); // �սڵ�

    // �ݹ鴦��FBX�ڵ�
    for (int i = 0; i < fbxRootNode->GetChildCount(); ++i) {
        FbxNode* child = fbxRootNode->GetChild(i);
        ProcessFbxNode(child, rootNode);
    }

    return rootNode;
}

// �ݹ鴦��FBX�ڵ㲢����SceneNode
SceneNode* FBXManager::ProcessFbxNode(FbxNode* fbxNode, SceneNode* parentNode)
{
    std::string nodeName = fbxNode->GetName();
    glm::mat4 localTransform = ConvertFbxTransform(fbxNode);

    // ���ڵ��Ƿ��������
    FbxMesh* fbxMesh = fbxNode->GetMesh();
    Entity* entity = nullptr;
    MeshComponent* meshComponent = nullptr;
    if (fbxMesh) {
        entity = new Entity(nodeName);
        meshComponent = ResourceManager::LoadFBXMesh(fbxMesh, m_Scene->GetName()); // ��Ҫʵ��LoadFBXMesh
        if (meshComponent) {
            entity->AddComponent(meshComponent);
        }
    }

    // ���ڵ��Ƿ��ǹ�Դ
    Light* light = nullptr;
    ProcessLight(fbxNode, &light);

    // ����SceneNode
    SceneNode* sceneNode = nullptr;
    if (entity) {
        sceneNode = new SceneNode(nodeName, entity, parentNode);
    }
    else if (light) {
        // ���ݹ�Դ���ͣ�������Ӧ��SceneNode���캯��
        // ����ֻ������Դ��Ϊʾ�����������չ����������Դ����
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
        // ����ڵ�Ȳ�������Ҳ���ǹ�Դ�����Դ���һ���յ�SceneNode
        sceneNode = new SceneNode(nodeName, static_cast<void*>(nullptr), parentNode);
    }

    // �ݹ鴦���ӽڵ�
    for (int i = 0; i < fbxNode->GetChildCount(); ++i) {
        FbxNode* child = fbxNode->GetChild(i);
        ProcessFbxNode(child, sceneNode);
    }

    return sceneNode;
}

// �������񲢷���MeshComponent
void FBXManager::ProcessMesh(FbxMesh* fbxMesh, Entity* entity)
{
    MeshComponent* meshComponent = ResourceManager::LoadFBXMesh(fbxMesh, m_Scene->GetName());
    if (meshComponent) {
        entity->AddComponent(meshComponent);
    }
}

// ������ʲ�����PBRMaterial
PBRMaterial* FBXManager::ProcessMaterial(FbxSurfaceMaterial* fbxMaterial, const std::string& filePath)
{
    // ת��FBX���ʵ�PBRMaterial
    PBRMaterial* pbrMaterial = nullptr;

    pbrMaterial = ResourceManager::LoadPBRMaterial(fbxMaterial, filePath);

    return pbrMaterial;
}

// �����Դ������Light����
void FBXManager::ProcessLight(FbxNode* fbxNode, Light** light)
{
    FbxLight* fbxLight = fbxNode->GetLight();
    if (!fbxLight) {
        *light = nullptr;
        return;
    }

    // ��ȡ��Դ����
    FbxLight::EType lightType = fbxLight->LightType.Get();

    switch (lightType)
    {
    case FbxLight::ePoint:
    {
        // ����PointLight
        glm::vec3 color(
            static_cast<float>(fbxLight->Color.Get()[0]),
            static_cast<float>(fbxLight->Color.Get()[1]),
            static_cast<float>(fbxLight->Color.Get()[2])
        );
        float intensity = static_cast<float>(fbxLight->Intensity.Get());
        glm::vec3 position = glm::vec3(0.0f); // λ�ý���SceneNode�ı任������

        *light = new PointLight("PointLight_" + std::string(fbxNode->GetName()), color, intensity, position);
        break;
    }
    case FbxLight::eDirectional:
    {
        // ����DirectionalLight
        glm::vec3 color(
            static_cast<float>(fbxLight->Color.Get()[0]),
            static_cast<float>(fbxLight->Color.Get()[1]),
            static_cast<float>(fbxLight->Color.Get()[2])
        );
        float intensity = static_cast<float>(fbxLight->Intensity.Get());
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f); // ������SceneNode�ı任������

        *light = new DirectionalLight("DirectionalLight_" + std::string(fbxNode->GetName()), color, intensity, direction);
        break;
    }
    case FbxLight::eSpot:
    {
        // ����SpotLight
        glm::vec3 color(
            static_cast<float>(fbxLight->Color.Get()[0]),
            static_cast<float>(fbxLight->Color.Get()[1]),
            static_cast<float>(fbxLight->Color.Get()[2])
        );
        float intensity = static_cast<float>(fbxLight->Intensity.Get());
        glm::vec3 position = glm::vec3(0.0f); // λ�ý���SceneNode�ı任������
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f); // ������SceneNode�ı任������
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

// ת��FBX�ڵ�ı任��glm::mat4
glm::mat4 FBXManager::ConvertFbxTransform(FbxNode* fbxNode)
{
    FbxAMatrix fbxMatrix = fbxNode->EvaluateGlobalTransform();

    // FBX SDKʹ�õ�����������ϵ��GLMĬ������������ϵ��������Ҫת����
    // ����ֻ��һ���򵥵�ת��ʾ��������ת��ȡ���������������
    glm::mat4 glmMatrix = glm::mat4(
        static_cast<float>(fbxMatrix.Get(0, 0)), static_cast<float>(fbxMatrix.Get(0, 1)), static_cast<float>(fbxMatrix.Get(0, 2)), static_cast<float>(fbxMatrix.Get(0, 3)),
        static_cast<float>(fbxMatrix.Get(1, 0)), static_cast<float>(fbxMatrix.Get(1, 1)), static_cast<float>(fbxMatrix.Get(1, 2)), static_cast<float>(fbxMatrix.Get(1, 3)),
        static_cast<float>(fbxMatrix.Get(2, 0)), static_cast<float>(fbxMatrix.Get(2, 1)), static_cast<float>(fbxMatrix.Get(2, 2)), static_cast<float>(fbxMatrix.Get(2, 3)),
        static_cast<float>(fbxMatrix.Get(3, 0)), static_cast<float>(fbxMatrix.Get(3, 1)), static_cast<float>(fbxMatrix.Get(3, 2)), static_cast<float>(fbxMatrix.Get(3, 3))
    );

    return glmMatrix;
}
*/