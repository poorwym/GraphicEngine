#include "SceneManager.h"

// �޸ļ�����Ϊ std::string
std::map<std::string, Entity*> entityList;
std::map<std::string, SceneNode*> sceneNodeList;
std::map<std::string, LightController*> lightControllerList;
std::map<std::string, EntityController*> entityControllerList;
std::map<std::string, SceneNodeController*> sceneNodeControllerList;

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
    // ʹ�� std::string ��Ϊ��
    entityList[std::string(entityName)] = entity;
    sceneNodeList[std::string(sceneNodeName)] = node;
    entityControllerList[std::string(entityName)] = new EntityController(entity);
    sceneNodeControllerList[std::string(sceneNodeName)] = new SceneNodeController(node);
}