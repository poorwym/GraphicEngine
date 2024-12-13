#include "SceneManager.h"

std::map<std::string, Entity*> g_EntityList;
std::map<std::string, SceneNode*> g_SceneNodeList;
std::map<std::string, LightController*> g_LightControllerList;
std::map<std::string, EntityController*> g_EntityControllerList;
std::map<std::string, SceneNodeController*> g_SceneNodeControllerList;
std::map<std::string, PointLight*> g_PointLightList;
std::map<PointLight*, unsigned int> g_PointLightID;
std::map<std::string, SpotLight*> g_SpotLightList;
std::map<SpotLight*, unsigned int> g_SpotLightID;
std::vector<Texture*> g_TextureList;
std::map<unsigned int, int> g_TextureSlots;

bool needUpdate = false;

TextureArray* g_TextureArray = nullptr;

SceneManager::SceneManager(Scene* scene)
    :needUpdate(true)
{
    m_Scene = scene;
}

SceneManager::~SceneManager()
{
    // 删除所有 EntityController
    for (auto& pair : g_EntityControllerList) {
        delete pair.second;
    }
    g_EntityControllerList.clear();

    // 删除所有 SceneNodeController
    for (auto& pair : g_SceneNodeControllerList) {
        delete pair.second;
    }
    g_SceneNodeControllerList.clear();

    // 删除所有 LightController
    for (auto& pair : g_LightControllerList) {
        delete pair.second;
    }
    g_LightControllerList.clear();

    // 删除所有 PointLight
    for (auto& pair : g_PointLightList) {
        delete pair.second;
    }
    g_PointLightList.clear();

    // 删除所有 SceneNodes
    for (auto& pair : g_SceneNodeList) {
        delete pair.second;
    }
    g_SceneNodeList.clear();

    // 删除 all Entities
    for (auto& pair : g_EntityList) {
        delete pair.second;
    }
    g_EntityList.clear();
}


void SceneManager::AddEntity(MeshComponent* meshComponent, const char* entityName, const char* sceneNodeName, SceneNode* parent)
{
    Entity* entity = new Entity(entityName);
    entity->AddComponent(meshComponent);
    SceneNode* node = new SceneNode(sceneNodeName, entity, nullptr);
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
    // 使用 std::string 作为键
    g_EntityList[std::string(entityName)] = entity;
    g_SceneNodeList[std::string(sceneNodeName)] = node;
    g_EntityControllerList[std::string(entityName)] = new EntityController(entity);
    g_SceneNodeControllerList[std::string(sceneNodeName)] = new SceneNodeController(node);
}

void SceneManager::AddPointLight(PointLight* light, const char* sceneNodeName, SceneNode* parent)
{
    SceneNode* node = new SceneNode(sceneNodeName, light, nullptr);
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
    g_PointLightList[light->GetName()] = light;
    g_PointLightID[light] = g_PointLightList.size() - 1;
    PointLightController* pointLightController = new PointLightController(light);
    g_LightControllerList[light->GetName()] = pointLightController;
    g_SceneNodeControllerList[std::string(sceneNodeName)] = new SceneNodeController(node);
}

void SceneManager::AddTexture(Texture* texture)
{
    g_TextureList.push_back(texture);
    if(g_TextureSlots.find(texture->GetTextureID()) == g_TextureSlots.end())  g_TextureSlots[texture->GetTextureID()] = g_TextureSlots.size();
}

int SceneManager::AddTexture(const char* path)
{
    static int counter = 0;
    g_TextureArray->AddTexture(path);
    counter++;
    return counter - 1;
}