// ComputeShader.cpp

#include "ComputeShader.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include "Renderer.h"

// 辅助函数：处理 #include 指令，支持递归
static std::string ProcessIncludes(const std::string& shaderCode, const std::string& directory, std::unordered_set<std::string>& includedFiles, int depth = 0, int maxDepth = 10) {
    if (depth > maxDepth) {
        std::cerr << "Error: Exceeded maximum include depth." << std::endl;
        return "";
    }

    std::stringstream processed;
    std::istringstream stream(shaderCode);
    std::string line;

    while (std::getline(stream, line)) {
        size_t includePos = line.find("#include");
        if (includePos != std::string::npos) {
            size_t firstQuote = line.find("\"", includePos);
            size_t lastQuote = line.find("\"", firstQuote + 1);
            if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote) {
                std::string includeFile = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                std::string includePath = directory + "/" + includeFile;

                if (includedFiles.find(includePath) != includedFiles.end()) {
                    std::cerr << "Warning: File already included: " << includePath << std::endl;
                    continue;
                }

                includedFiles.insert(includePath);

                std::ifstream includeStream(includePath);
                if (!includeStream.is_open()) {
                    std::cerr << "Error: Unable to open include file: " << includePath << std::endl;
                    continue;
                }

                std::stringstream includeContent;
                includeContent << includeStream.rdbuf();
                includeStream.close();

                std::string includedCode = ProcessIncludes(includeContent.str(), directory, includedFiles, depth + 1, maxDepth);
                processed << includedCode << "\n";
            }
            else {
                processed << line << "\n";
            }
        }
        else {
            processed << line << "\n";
        }
    }

    return processed.str();
}

ComputeShader::ComputeShader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    std::string computeShaderSource = ParseShader(filepath);
    m_RendererID = CreateShader(computeShaderSource);
}

ComputeShader::~ComputeShader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void ComputeShader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void ComputeShader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void ComputeShader::Dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const
{
    GLCall(glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ));
    GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
}

unsigned int ComputeShader::CreateShader(const std::string& computeShaderSource)
{
    unsigned int program = glCreateProgram();
    unsigned int cs = CompileShader(computeShaderSource);

    GLCall(glAttachShader(program, cs));
    GLCall(glLinkProgram(program));

    int result;
    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &result));
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetProgramInfoLog(program, length, &length, message));
        std::cout << "Failed to link compute shader program!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteProgram(program));
        return 0;
    }

    GLCall(glValidateProgram(program));
    GLCall(glDeleteShader(cs));

    return program;
}

unsigned int ComputeShader::CompileShader(const std::string& source)
{
    unsigned int id = glCreateShader(GL_COMPUTE_SHADER);
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile compute shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

std::string ComputeShader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        std::cerr << "Failed to open compute shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream ss;
    std::string line;
    std::unordered_set<std::string> includedFiles;

    size_t lastSlash = filepath.find_last_of("/\\");
    std::string directory = (lastSlash != std::string::npos) ? filepath.substr(0, lastSlash) : ".";

    while (std::getline(stream, line)) {
        ss << line << '\n';
    }
    stream.close();

    std::string shaderCode = ss.str();
    shaderCode = ProcessIncludes(shaderCode, directory, includedFiles);

    return shaderCode;
}

int ComputeShader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1) {
        std::cout << "Warning: uniform '" << name << "' doesn't exist in compute shader!" << std::endl;
    }

    m_UniformLocationCache[name] = location;
    return location;
}

void ComputeShader::SetUniform1i(const std::string& name, int v0)
{
    GLCall(glUniform1i(GetUniformLocation(name), v0));
}

void ComputeShader::SetUniform1f(const std::string& name, float v0)
{
    GLCall(glUniform1f(GetUniformLocation(name), v0));
}

void ComputeShader::SetUniform2f(const std::string& name, float v0, float v1)
{
    GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}

void ComputeShader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void ComputeShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void ComputeShader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void ComputeShader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix)
{
    GLCall(glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}