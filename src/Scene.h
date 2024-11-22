#pragma once
#include <string>
#include "Entity.h"
#include <vector>
#include "SceneNode.h"
#include "Camera.h"
#include <map>

class Scene {
private:
    std::map<std::string, SceneNode *> m_SceneNodes;
    DirectionalLight* m_DirLight;
    unsigned int m_RendererID;
    VertexArray* m_VAO;
    VertexBuffer* m_VBO;
    IndexBuffer* m_IBO;
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    void UpdateVAO();
public:
    Scene();
    void load(const std::string& filePath);   // 加载场景
    void save(const std::string& filePath);   // 保存场景
    void Update(float deltaTime);             // 更新场景
    void AddNode(SceneNode* node);           // 添加实体
    void SetDirectionalLight(DirectionalLight* dirLight);
    void BindLight(Shader& shader, glm::mat4 globalTransform);
    void RenderDepthMap(Shader& shader);
    void RenderShadowMap(Shader* depthShader, Shader* cubeDepthShader);
    void BatchRender(Shader& shader, Camera& camera);
    void Render(Shader& shader,Camera& camera);
    void RemoveNode(SceneNode* node);        // 删除实体
    void OnImGuiTree();
    inline DirectionalLight* GetDirectionalLight() const {return m_DirLight;}
};