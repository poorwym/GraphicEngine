
#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

class ComputeShader
{
private:
    std::string m_FilePath;
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;

public:
    ComputeShader(const std::string& filepath);
    ~ComputeShader();

    void Bind() const;
    void Unbind() const;

    // 调度计算着色器
    void Dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const;

    // Uniform 设置函数
    void SetUniform1i(const std::string& name, int v0);
    void SetUniform1f(const std::string& name, float v0);
    void SetUniform2f(const std::string& name, float v0, float v1);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat3f(const std::string& name, const glm::mat3& matrix);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
    unsigned int CreateShader(const std::string& computeShaderSource);
    unsigned int CompileShader(const std::string& source);
    std::string ParseShader(const std::string& filepath);
    int GetUniformLocation(const std::string& name);
};