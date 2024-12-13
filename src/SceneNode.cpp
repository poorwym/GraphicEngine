#include "SceneNode.h"
#include "Entity.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "SceneManager.h"
#include "EngineState.h"

void SceneNode::AddChild(SceneNode* child)
{
    m_Children[child->GetName()] = child;
}

void SceneNode::RemoveChild(SceneNode* child)
{
    m_Children[child->GetName()] = nullptr;
}

void SceneNode::BindLight(Shader& shader, glm::mat4 globalTranform)
{
    glm::mat4 m_GlobalTransform = globalTranform * m_LocalTransform;
    if(m_PointLight){
        m_PointLight->Bind(shader, m_GlobalTransform);
    }
    for (auto& pair : m_Children) {
        pair.second->BindLight(shader, m_GlobalTransform);
    }
}

void SceneNode::RenderDepthMap(Shader& shader, glm::mat4 globalTranform)
{
    glm::mat4 m_GlobalTransform = globalTranform * m_LocalTransform;
    if (m_Entity) {
        m_Entity->RenderDepthMap(shader, m_GlobalTransform);
    }
    for (auto& pair : m_Children) {
        pair.second->RenderDepthMap(shader, m_GlobalTransform);
    }
}

void SceneNode::Render(Shader& shader, Camera& camera, glm::mat4 globalTranform)
{
    glm::mat4 m_GlobalTransform = globalTranform * m_LocalTransform;
    if (m_Entity){
        m_Entity->Render(shader, camera, m_GlobalTransform);
    }
    for (auto& pair : m_Children) {
        pair.second->Render(shader, camera, m_GlobalTransform);
    }
}

void SceneNode::SetPosition(glm::vec3 position)
{
    m_Position = position;
}

void SceneNode::SetRotation(glm::vec3 rotation)
{
    m_Rotation = rotation;
}
extern EngineState engineState;

void SceneNode::Update(float deltaTime)
{
    glm::mat4 transform(1.0f);
    // 应用缩放
    transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

    // 应用平移
    transform = glm::translate(transform, m_Position);

    m_LocalTransform = transform;
    if (m_LocalTransform != m_PreviousTransform) {
        m_PreviousTransform = m_LocalTransform;
        engineState.needUpdate = true;
    }
    if(m_Entity)
        m_Entity->Update(deltaTime);
    if(m_PointLight)
        m_PointLight->Update(deltaTime);
    for (auto& pair : m_Children)
    {
        pair.second->Update(deltaTime);
    }
}

void SceneNode::OnImGuiTree()
{
    if (ImGui::TreeNode(m_Name.c_str()))
    {

        if (ImGui::Button("Selected the Node"))
        {
            ImGui::OpenPopup(m_Name.c_str());
        }
        if (ImGui::BeginPopup(m_Name.c_str()))
        {
            g_SceneNodeControllerList[m_Name]->OnImGuiRender();
            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        //展示所有点光源
        if(m_PointLight){
            if (ImGui::Button(m_PointLight->GetName().c_str()))
            {
                ImGui::OpenPopup(m_PointLight->GetName().c_str());
            }
            if (ImGui::BeginPopup(m_PointLight->GetName().c_str()))
            {
                g_LightControllerList[m_PointLight->GetName()]->OnImGuiRender();
                if (ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        //展示所有实体
        if(m_Entity){
            if (ImGui::Button(m_Entity->GetName().c_str()))
            {
                ImGui::OpenPopup(m_Entity->GetName().c_str());
            }
            if (ImGui::BeginPopup(m_Entity->GetName().c_str()))
            {
                g_EntityControllerList[m_Entity->GetName()]->OnImGuiRender();
                if (ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        for (auto& pair : m_Children) {
            pair.second->OnImGuiTree();
        }
        ImGui::TreePop();
    }
}

std::vector<std::vector<Vertex>*> SceneNode::GetVertices(glm::mat4 globalTranform)
{
    std::vector<std::vector<Vertex>*> vertices;
    glm::mat4 m_GlobalTransform = globalTranform * m_LocalTransform;
    if (m_Entity) {
        std::vector<std::vector<Vertex>*> entityVertices = m_Entity->GetVertices(m_GlobalTransform);
        vertices.insert(vertices.end(), entityVertices.begin(), entityVertices.end());
    }
    for (auto& pair : m_Children) {
        std::vector<std::vector<Vertex>*> childVertices = pair.second->GetVertices(m_GlobalTransform);
        vertices.insert(vertices.end(), childVertices.begin(), childVertices.end());
    }
    return vertices;
}

std::vector<std::vector<unsigned int>*> SceneNode::GetIndices()
{
    std::vector<std::vector<unsigned int>*> indices;
    if (m_Entity) {
        std::vector<std::vector<unsigned int>*> entityIndices = m_Entity->GetIndices();
        indices.insert(indices.end(), entityIndices.begin(), entityIndices.end());
    }
    for (auto& pair : m_Children) {
        std::vector<std::vector<unsigned int>*> childIndices = pair.second->GetIndices();
        indices.insert(indices.end(), childIndices.begin(), childIndices.end());
    }
    return indices;
}

glm::mat4 SceneNode::GetGlobalTransform() const
{
    return m_Parent ? m_Parent->GetGlobalTransform() * GetLocalTransform() : GetLocalTransform();
}

SceneNode::SceneNode(std::string name, void* ptr, SceneNode* parent)
    :m_LocalTransform(glm::mat4(1.0f)), m_Name(name), m_Position(glm::vec3(0.0f)), m_Rotation(glm::vec3(0.0f))
{
    m_Entity = nullptr;
    m_DirectionalLight = nullptr;
    m_SpotLight = nullptr;
    m_PointLight = nullptr;
    m_Parent = parent;
}

SceneNode::SceneNode(std::string name, Entity* entity, SceneNode* parent)
    :m_LocalTransform(glm::mat4(1.0f)),m_Name(name),m_Position(glm::vec3(0.0f)), m_Rotation(glm::vec3(0.0f))
{
    m_Entity = entity;
    m_DirectionalLight = nullptr;
    m_SpotLight = nullptr;
    m_PointLight = nullptr;
    m_Parent = parent;
}

SceneNode::SceneNode(std::string name, PointLight* light, SceneNode* parent)
    :m_LocalTransform(glm::mat4(1.0f)), m_Name(name), m_Position(glm::vec3(0.0f)), m_Rotation(glm::vec3(0.0f))
{
    m_Entity = nullptr;
    m_DirectionalLight = nullptr;
    m_SpotLight = nullptr;
    m_PointLight = light;
    m_Parent = parent;
}

SceneNode::SceneNode(std::string name, DirectionalLight* directionalLight, SceneNode* parent)
    :m_LocalTransform(glm::mat4(1.0f)), m_Name(name), m_Position(glm::vec3(0.0f)), m_Rotation(glm::vec3(0.0f))
{
    m_Entity = nullptr;
    m_DirectionalLight = directionalLight;
    m_SpotLight = nullptr;
    m_PointLight = nullptr;
    m_Parent = parent;
}

SceneNode::SceneNode(std::string name, SpotLight* spotLight, SceneNode* parent)
    :m_LocalTransform(glm::mat4(1.0f)), m_Name(name), m_Position(glm::vec3(0.0f)), m_Rotation(glm::vec3(0.0f))
{
    m_Entity = nullptr;
    m_DirectionalLight = nullptr;
    m_SpotLight = spotLight;
    m_PointLight = nullptr;
    m_Parent = parent;
}

SceneNode::~SceneNode()
{
    if (m_Entity) {
        delete m_Entity;
    }
    if (m_PointLight) {
        delete m_PointLight;
    }
    for (auto& pair : m_Children) {
        delete pair.second;
    }
    m_Children.clear();
}

glm::mat4 SceneNode::GetLocalTransform() const
{
    return m_Entity ? m_Entity->GetLocalTransform() : glm::mat4(1.0f);
}