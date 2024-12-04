#include "Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include "Renderer.h"

// 辅助函数：递归处理 #include 指令，避免重复包含和无限递归
std::string ProcessIncludes(const std::string& shaderCode, const std::string& directory, std::unordered_set<std::string>& includedFiles, int depth = 0, int maxDepth = 10) {
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

                // 检查是否已经包含过该文件，防止重复包含
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

                // 递归处理包含文件中的 #include 指令
                std::string includedCode = ProcessIncludes(includeContent.str(), directory, includedFiles, depth + 1, maxDepth);
                processed << includedCode << "\n";
                //std::cout << "includeCode:" << includedCode << std::endl;
            }
            else {
                // 如果 #include 指令格式不正确，直接输出该行
                processed << line << "\n";
            }
        }
        else {
            // 非 #include 行，直接输出
            processed << line << "\n";
        }
    }

    return processed.str();
}

Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return { "", "" };
    }

    std::string line;
    std::stringstream ss[2];
    std::unordered_set<std::string> includedFiles;

    // 获取主着色器文件所在目录
    size_t lastSlash = filepath.find_last_of("/\\");
    std::string directory = (lastSlash != std::string::npos) ? filepath.substr(0, lastSlash) : ".";

    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
            else {
                type = ShaderType::NONE;
            }
        }
        else {
            if (type != ShaderType::NONE) {
                ss[(int)type] << line << '\n';
            }
        }
    }
    stream.close();

    // 处理顶点着色器中的 #include
    std::string vertexCode = ss[0].str();
    vertexCode = ProcessIncludes(vertexCode, directory, includedFiles);

    // 清除已包含的文件集合，以便片段着色器可以重新包含相同的文件（如果需要）
    includedFiles.clear();

    // 处理片段着色器中的 #include
    std::string fragmentCode = ss[1].str();
    fragmentCode = ProcessIncludes(fragmentCode, directory, includedFiles);

    return { vertexCode, fragmentCode };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
#ifdef _DEBUG
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Fail to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << "shader :" << m_FilePath << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
#endif
    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::setUniformVec3f(const std::string& name, const glm::vec3& value)
{
    int location = GetUniformLocation(name);
    if (location != -1) { // 确保 Uniform 存在
        GLCall(glUniform3f(location, value.x, value.y, value.z));
    }
}

void Shader::setUniform1i(const std::string& name, int v0)
{
    GLCall(glUniform1i(GetUniformLocation(name), v0));
}

void Shader::setUniform1f(const std::string& name, float v0)
{
    GLCall(glUniform1f(GetUniformLocation(name), v0));
}

void Shader::setUniform2f(const std::string& name, float v0, float v1)
{
    GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2)
{
    GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }
    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1) {
        std::cout << "Warning: uniform " << name << " doesn't exist!" << " Shader: " << m_FilePath << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
}