#include "SceneManager.h"

std::map<std::string, Entity*> entityList;
std::map<std::string, SceneNode*> sceneNodeList;
std::map<std::string, LightController*> lightControllerList;
std::map<std::string, EntityController*> entityControllerList;
std::map<std::string, SceneNodeController*> sceneNodeControllerList;
std::map<std::string, PointLight*> pointLightList;
std::map<PointLight*, unsigned int> pointLightID;
std::vector<Texture*> textureList;
std::map<unsigned int, int> textureSlots;

TextureArray* textureArray = nullptr;

SceneManager::SceneManager(Scene* scene)
{
    m_Scene = scene;
}

SceneManager::~SceneManager()
{
    // ɾ������ EntityController
    for (auto& pair : entityControllerList) {
        delete pair.second;
    }
    entityControllerList.clear();

    // ɾ������ SceneNodeController
    for (auto& pair : sceneNodeControllerList) {
        delete pair.second;
    }
    sceneNodeControllerList.clear();

    // ɾ������ LightController
    for (auto& pair : lightControllerList) {
        delete pair.second;
    }
    lightControllerList.clear();

    // ɾ������ PointLight
    for (auto& pair : pointLightList) {
        delete pair.second;
    }
    pointLightList.clear();

    // ɾ������ SceneNodes
    for (auto& pair : sceneNodeList) {
        delete pair.second;
    }
    sceneNodeList.clear();

    // ɾ�� all Entities
    for (auto& pair : entityList) {
        delete pair.second;
    }
    entityList.clear();
}


void SceneManager::AddEntity(MeshComponent* meshComponent, const char* entityName, const char* sceneNodeName, SceneNode* parent)
{
    Entity* entity = new Entity(entityName);
    entity->AddComponent(meshComponent);
    SceneNode* node = new SceneNode(sceneNodeName, entity, nullptr);
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
    // ʹ�� std::string ��Ϊ��
    entityList[std::string(entityName)] = entity;
    sceneNodeList[std::string(sceneNodeName)] = node;
    entityControllerList[std::string(entityName)] = new EntityController(entity);
    sceneNodeControllerList[std::string(sceneNodeName)] = new SceneNodeController(node);
}

void SceneManager::AddPointLight(PointLight* light, const char* sceneNodeName, SceneNode* parent)
{
    SceneNode* node = new SceneNode(sceneNodeName, light, nullptr);
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
    pointLightList[light->GetName()] = light;
    pointLightID[light] = pointLightList.size() - 1;
    PointLightController* pointLightController = new PointLightController(light);
    lightControllerList[light->GetName()] = pointLightController;
    sceneNodeControllerList[std::string(sceneNodeName)] = new SceneNodeController(node);
}

void SceneManager::AddTexture(Texture* texture)
{
    textureList.push_back(texture);
    if(textureSlots.find(texture->GetTextureID()) == textureSlots.end())  textureSlots[texture->GetTextureID()] = textureSlots.size();
}

int SceneManager::AddTexture(const char* path)
{
    static int counter = 0;
    textureArray->AddTexture(path);
    counter++;
    return counter - 1;
}