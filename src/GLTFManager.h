// GLTFManager.h
#pragma once

#include "SceneNode.h"
#include "Entity.h"
#include "Component.h"
#include "Material.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"
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

    // ���� GLTF �ļ������ظ��ڵ�
    SceneNode* Load(const std::string& filePath, const std::string& fileName, float scaleRate = 1.0f);

private:
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    // �����棬�����ظ�����
    std::map<int, unsigned int> textureCache;

    // ��������
    bool LoadModel(const std::string& filePath, const std::string& fileName);
    SceneNode * ProcessNode(int nodeIndex, SceneNode* parent, float scaleRate, const std::string& filePath);
    MeshComponent * ProcessMesh(int meshIndex, float scaleRate, const std::string& filePath);
    PBRMaterial* ProcessMaterial(int materialIndex, const std::string& filePath);
    glm::mat4 GetTransform(const tinygltf::Node& node);
};