#pragma once
#include "Scene.h"
#include "LightController.h"
#include "EntityController.h"
class SceneManager {
private:
	Scene* m_Scene;
public:
	SceneManager(Scene* scene);
	void AddEntity(Mesh* mesh, const char* entityName, const char* sceneNodeName);
	void AddEntity(Mesh* mesh, const char* entityName, const char* sceneNodeName, SceneNode* parent);
};

extern std::map<const char*, Entity*> entityList;
extern std::map<const char*, SceneNode*> sceneNodeList;
extern std::map<const char*, LightController*> lightControllerList;
extern std::map<const char*, EntityController*> entityControllerList;
