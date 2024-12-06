#pragma once
#include "Scene.h"
#include "LightController.h"
#include "EntityController.h"
#include "SceneNodeController.h"
#include <string>
#include "TextureArray.h"


class SceneManager {
private:
	Scene* m_Scene;
	bool needUpdate;
public:
	SceneManager(Scene* scene);
	~SceneManager();
	void AddEntity(MeshComponent* meshComponent, const char* entityName, const char* sceneNodeName, SceneNode* parent);
	void AddPointLight(PointLight* light, const char* sceneNodeName, SceneNode* parent);
	void AddTexture(Texture* texture);
	int AddTexture(const char* fileName);
};

extern std::map<std::string, Entity*> entityList;
extern std::map<std::string, SceneNode*> sceneNodeList;
extern std::map<std::string, LightController*> lightControllerList;
extern std::map<std::string, EntityController*> entityControllerList;
extern std::map<std::string, SceneNodeController*> sceneNodeControllerList;
extern std::map<std::string, PointLight*> pointLightList;
extern std::map<PointLight*, unsigned int> pointLightID;
extern std::vector<Texture*> textureList;
extern std::map<unsigned int, int> textureSlots;

extern TextureArray* textureArray;