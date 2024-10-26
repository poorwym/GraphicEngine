#pragma once
#include "Scene.h"
#include "LightController.h"
#include "EntityController.h"
#include "SceneNodeController.h"
#include <string>
class SceneManager {
private:
	Scene* m_Scene;
public:
	SceneManager(Scene* scene);
	void AddEntity(Mesh* mesh, const char* entityName, const char* sceneNodeName, SceneNode* parent);
};

extern std::map<std::string, Entity*> entityList;
extern std::map<std::string, SceneNode*> sceneNodeList;
extern std::map<std::string, LightController*> lightControllerList;
extern std::map<std::string, EntityController*> entityControllerList;
extern std::map<std::string, SceneNodeController*> sceneNodeControllerList;
