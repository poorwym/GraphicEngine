#pragma once
#include "Scene.h"
#include "LightController.h"
#include "EntityController.h"
#include "SceneNodeController.h"
#include "SceneNode.h"
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
	void AddSceneNode(SceneNode* node, SceneNode* parent);
	void AddDirectionalLight(DirectionalLight* dirLight, const char* sceneNodeName, SceneNode* parent);
	void AddSpotLight(SpotLight* light, const char* sceneNodeName, SceneNode* parent);

	void AddTexture(Texture* texture);
	int AddTexture(const char* fileName);
	void AddMaterial(Material& material);
	int GetMaterialIndex(Material& material);
};

extern std::map<std::string, PointLight*> g_PointLightList;
extern std::map<PointLight*, unsigned int> g_PointLightID;
extern std::vector<Texture*> g_TextureList;
extern std::map<unsigned int, int> g_TextureSlots;
extern std::map<std::string, SpotLight*> g_SpotLightList;
extern std::map<SpotLight*, unsigned int> g_SpotLightID;
extern std::map<std::string, DirectionalLight*> g_DirectionalLightList;
extern std::map<DirectionalLight*, unsigned int> g_DirectionalLightID;

extern TextureArray* g_TextureArray;
extern std::vector<Material> g_MaterialList;