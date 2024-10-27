#include "SceneManager.h"

// 修改键类型为 std::string
std::map<std::string, Entity*> entityList;
std::map<std::string, SceneNode*> sceneNodeList;
std::map<std::string, LightController*> lightControllerList;
std::map<std::string, EntityController*> entityControllerList;
std::map<std::string, SceneNodeController*> sceneNodeControllerList;
std::map<std::string, PointLight *> pointLightList;
std::map<PointLight *,unsigned int> pointLightID;

SceneManager::SceneManager(Scene* scene)
{
    m_Scene = scene;
}

void SceneManager::AddEntity(Mesh* mesh, const char* entityName, const char* sceneNodeName, SceneNode* parent)
{
    Entity* entity = new Entity(entityName);
    entity->AddComponent(new MeshComponent(mesh));
    SceneNode* node = new SceneNode(sceneNodeName, entity, nullptr);
    parent ? parent->AddChild(node) : m_Scene->AddNode(node);
    // 使用 std::string 作为键
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
}
