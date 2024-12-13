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

    // ����FBX�ļ������ظ��ڵ�
    SceneNode* Load(const std::string& filePath, const std::string& fileName);

private:
    // FBX SDK �������ͳ���
    FbxManager* m_Manager;
    FbxScene* m_Scene;

    // ��������
    void InitializeSdkObjects(FbxManager*& manager, FbxScene*& scene);
    bool LoadScene(const std::string& filePath, const std::string& fileName);

    // �ݹ����FBX�ڵ㲢����SceneNode
    SceneNode* ProcessFbxNode(FbxNode* fbxNode, SceneNode* parentNode = nullptr);

    // ��������
    void ProcessMesh(FbxMesh* fbxMesh, Entity* entity);

    // �������
    PBRMaterial* ProcessMaterial(FbxSurfaceMaterial* fbxMaterial, const std::string& filePath);

    // �����Դ
    void ProcessLight(FbxNode* fbxNode, Light** light);

    // ת��FBX�任��glm::mat4
    glm::mat4 ConvertFbxTransform(FbxNode* fbxNode);

    // �����ӽڵ�
    void ProcessChildren(FbxNode* fbxNode, SceneNode* parentNode);
};
*/