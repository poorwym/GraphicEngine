#pragma once
/*
#include <string>
#include "SceneNode.h"
#include <fbxsdk.h>

class FBXManager
{
public:
    FBXManager();
    ~FBXManager();

    // 加载FBX文件并返回根节点
    SceneNode* Load(const std::string& filePath, const std::string& fileName);

private:
    // FBX SDK 管理器和场景
    FbxManager* m_Manager;
    FbxScene* m_Scene;

    // 辅助函数
    void InitializeSdkObjects(FbxManager*& manager, FbxScene*& scene);
    bool LoadScene(const std::string& filePath, const std::string& fileName);

    // 递归遍历FBX节点并创建SceneNode
    SceneNode* ProcessFbxNode(FbxNode* fbxNode, SceneNode* parentNode = nullptr);

    // 处理网格
    void ProcessMesh(FbxMesh* fbxMesh, Entity* entity);

    // 处理材质
    PBRMaterial* ProcessMaterial(FbxSurfaceMaterial* fbxMaterial, const std::string& filePath);

    // 处理光源
    void ProcessLight(FbxNode* fbxNode, Light** light);

    // 转换FBX变换到glm::mat4
    glm::mat4 ConvertFbxTransform(FbxNode* fbxNode);

    // 处理子节点
    void ProcessChildren(FbxNode* fbxNode, SceneNode* parentNode);
};
*/