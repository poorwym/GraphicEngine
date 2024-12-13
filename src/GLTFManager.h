// GLTFManager.h
#pragma once

#include "SceneNode.h"
#include "Entity.h"
#include "Component.h"
#include "Material.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"

#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif


#include "tinyglTF/tiny_gltf.h"
#include "TextureManager.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class GLTFManager {
public:
    GLTFManager();
    ~GLTFManager();

    // 加载 GLTF 文件并返回根节点
    SceneNode* LoadGLTF(const std::string& filePath, const std::string& fileName, float scaleRate = 1.0f);

private:
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    // 纹理缓存，避免重复加载
    std::map<int, unsigned int> textureCache;

    // 辅助函数
    bool LoadModel(const std::string& filePath, const std::string& fileName);
    SceneNode* ProcessNode(int nodeIndex, SceneNode* parent, float scaleRate);
    MeshComponent* ProcessMesh(int meshIndex, float scaleRate);
    PBRMaterial* ProcessMaterial(int materialIndex, const std::string& filePath);
    glm::mat4 GetTransform(const tinygltf::Node& node);
};