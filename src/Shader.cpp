#include "Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Renderer.h"


Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath),m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}
ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
    enum class ShaderType {//一种枚举类，用法可类比宏，ShaderType::NONE=-1
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }

        }
        else {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str() };
}
unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {//传入shader的类型和源代码，返回shader的id
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();//把源代码转化为指针,c_str()是个指向source的指针
    glShaderSource(id, 1, &src, nullptr);//把源代码传给shader,id为shader的id，1表示只有一个源代码，&src为源代码的指针的地址，nullptr表示没有附加信息
    glCompileShader(id);//编译shader 

    int result;
#ifdef _DEBUG
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);//获取编译状态
    if (result == GL_FALSE) {//编译失败
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);//获取编译信息长度,&length表示要获取的信息的长度
        char* message = (char*)alloca(length * sizeof(char));//分配内存
        glGetShaderInfoLog(id, length, &length, message);//获取编译信息，把编译信息传给message
        std::cout << "Fail to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << "shader " << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
#endif
    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {//建立shader,传入顶点shader和片段shader
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);//创造一个顶点shader，GL_VERTEX_SHADER表示类型
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);//创造一个片段shader,GL_FRAGMENT_SHADER表示类型

    glAttachShader(program, vs);//把顶点shader和program链接到一起
    glAttachShader(program, fs);//把片段shader和program链接到一起

    glLinkProgram(program);

    glDeleteShader(vs);//删除顶点shader,因为已经和program链接，所以可以删除
    glDeleteShader(fs);//删除片段shader,因为已经和program链接，所以可以删除

    return program; //返回program的id
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
    GLCall(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z));
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
		std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;
    }
	m_UniformLocationCache[name] = location;
	return location;
}
