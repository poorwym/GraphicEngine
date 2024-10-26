#include "SceneManager.h"

std::map<const char*, Entity*> entityList;
std::map<const char*, SceneNode*> sceneNodeList;
std::map<const char*, LightController*> lightControllerList;
std::map<const char*, EntityController*> entityControllerList;

SceneManager::SceneManager(Scene* scene)
{
	m_Scene = scene;
}

void SceneManager::AddEntity(Mesh* mesh, const char* entityName, const char* sceneNodeName)
{
    Entity* entity = new Entity(entityName);
    entity->AddComponent(new MeshComponent(mesh));
    SceneNode* node = new SceneNode(sceneNodeName, entity, nullptr);
    m_Scene->AddNode(node);

    EntityController* entityController = new EntityController(entity);

    entityList[entityName]=entity;
    sceneNodeList[sceneNodeName]=node;
    entityControllerList[entityName]=entityController;
}

void SceneManager::AddEntity(Mesh* mesh, const char* entityName, const char* sceneNodeName, SceneNode* parent)
{
    Entity* entity = new Entity(entityName);
    entity->AddComponent(new MeshComponent(mesh));
    SceneNode* node = new SceneNode(sceneNodeName, entity, nullptr);
    parent->AddChild(node);


    EntityController* entityController = new EntityController(entity);

    entityList[entityName] = entity;
    sceneNodeList[sceneNodeName] = node;
    entityControllerList[entityName] = entityController;
}
