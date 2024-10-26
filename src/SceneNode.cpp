#include "SceneNode.h"
#include "Entity.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SceneManager.h"

void SceneNode::AddChild(SceneNode* child)
{
    m_Children[child->GetName()] = child;
}

void SceneNode::RemoveChild(SceneNode* child)
{
    m_Children[child->GetName()] = nullptr;
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
    m_Entity->Update(deltaTime);
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
            sceneNodeControllerList[m_Name]->OnImGuiRender();
            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::Button(m_Entity->getName().c_str()))
        {
            ImGui::OpenPopup(m_Entity->getName().c_str());
        }
        if (ImGui::BeginPopup(m_Entity->getName().c_str()))
        {
            entityControllerList[m_Entity->getName()]->OnImGuiRender();
            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        for (auto& pair : m_Children) {
            pair.second->OnImGuiTree();
        }
        ImGui::TreePop();
    }

}

glm::mat4 SceneNode::GetGlobalTransform() const
{
    return m_Parent ? m_Parent->GetGlobalTransform() * GetLocalTransform() : GetLocalTransform();
}

SceneNode::SceneNode(std::string name, Entity* entity, SceneNode* parent)
    :m_LocalTransform(glm::mat4(1.0f)),m_Name(name),m_Position(glm::vec3(0.0f)), m_Rotation(glm::vec3(0.0f))
{
    m_Entity = entity;
    m_Parent = parent;
}

glm::mat4 SceneNode::GetLocalTransform() const
{
    return m_Entity ? m_Entity->GetLocalTransform() : glm::mat4(1.0f);
}