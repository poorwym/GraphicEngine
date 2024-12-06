#pragma once
#include <string>
#include "Entity.h"
#include <vector>
#include "SceneNode.h"
#include "Camera.h"
#include <map>
#include "Quad.h"

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
    void UpdateVBO();
    int numVertices = 0;
    
    TileQuad m_TileQuad;
    
public:
    Scene(int n);
    void UpdateVertices();
    void ResetVAO();
    void load(const std::string& filePath);   // ���س���
    void save(const std::string& filePath);   // ���泡��
    void Update(float deltaTime);             // ���³���
    void AddNode(SceneNode* node);           // ���ʵ��
    void SetDirectionalLight(DirectionalLight* dirLight);
    void BindLight(Shader& shader, glm::mat4 globalTransform);
    void RenderDepthMap(Shader& shader) const;
    void RenderShadowMap(Shader* depthShader, Shader* cubeDepthShader);
    void BatchRender(Shader& shader, Camera& camera);
    void RayTracingRender(Shader& shader, Camera& camera);
    void Render(Shader& shader,Camera& camera);
    void RemoveNode(SceneNode* node);        // ɾ��ʵ��
    void OnImGuiTree();
    inline DirectionalLight* GetDirectionalLight() const {return m_DirLight;}
    inline std::vector<Vertex>* GetVertices() {return &m_Vertices;}
    inline std::vector<unsigned int>* GetIndices() {return &m_Indices;}
};