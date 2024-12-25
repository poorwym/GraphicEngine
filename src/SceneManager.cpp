#include "SceneManager.h"

std::vector<Texture*> g_TextureList;
std::map<unsigned int, int> g_TextureSlots;
std::map<std::string, int> g_TextureIndex;

bool needUpdate = false;

TextureArray* g_TextureArray = nullptr;

SceneManager::SceneManager(Scene* scene)
    :needUpdate(true)
{
    m_Scene = scene;
}

SceneManager::~SceneManager()
{
}


void SceneManager::AddEntity(MeshComponent* meshComponent, const char* entityName, const char* sceneNodeName, SceneNode* parent)
{
    Entity* entity = new Entity(entityName);
    entity->AddComponent(meshComponent);
    SceneNode* node = new SceneNode(sceneNodeName, entity, nullptr);
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
}

void SceneManager::AddPointLight(PointLight* light, const char* sceneNodeName, SceneNode* parent)
{
    SceneNode* node = new SceneNode(sceneNodeName, light, nullptr);
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
    g_PointLightList[light->GetName()] = light;
    g_PointLightID[light] = g_PointLightList.size() - 1;
}

void SceneManager::AddSceneNode(SceneNode* node, SceneNode* parent)
{
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
}

void SceneManager::AddDirectionalLight(DirectionalLight* dirLight, const char* sceneNodeName, SceneNode* parent)
{
    SceneNode* node = new SceneNode(sceneNodeName, dirLight, nullptr);
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
    g_DirectionalLightList[dirLight->GetName()] = dirLight;
    g_DirectionalLightID[dirLight] = g_DirectionalLightList.size() - 1;
}

void SceneManager::AddSpotLight(SpotLight* light, const char* sceneNodeName, SceneNode* parent)
{
    SceneNode* node = new SceneNode(sceneNodeName, light, nullptr);
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
    g_SpotLightList[light->GetName()] = light;
    g_SpotLightID[light] = g_SpotLightList.size() - 1;
}

void SceneManager::AddTexture(Texture* texture)//discard
{
    g_TextureList.push_back(texture);
    if(g_TextureSlots.find(texture->GetTextureID()) == g_TextureSlots.end())  g_TextureSlots[texture->GetTextureID()] = g_TextureSlots.size();
}

int SceneManager::AddTexture(const char* path) // ·µ»ØÎÆÀíIndex
{
    if (g_TextureIndex.find(path) == g_TextureIndex.end()) { // Î´¼ÓÔØ
        g_TextureArray->AddTexture(path);
        g_TextureIndex[path] = g_TextureIndex.size();
    }
    return g_TextureIndex[path];
}
