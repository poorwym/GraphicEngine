#pragma once
#include <string>
#include "Scene.h"
#include <vector>
#include <map>
#include "GLTFManager.h"
#include "OBJManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"

class ModelManager
{
private:
    Scene* m_Scene;
    ResourceManager m_ResourceManager;
    SceneManager m_SceneManager;
    std::map<std::string, std::string> m_GLTFModelList;
    std::map<std::string, std::string> m_OBJModelList;
    std::map<std::string, float> m_ModelScale;

public:
    ModelManager(Scene* scene);
    ~ModelManager();
    inline void SetScene(Scene* scene) { m_Scene = scene; }
    void OnImGuiRender();
    void Load(const std::string& filePath, const std::string& fileName, const std::string& type);

    // 方法用于添加模型到列表
    void AddModel(const std::string& type, const std::string& filePath, const std::string& fileName, float scale);
};