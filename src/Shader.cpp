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
    enum class ShaderType {//һ��ö���࣬�÷�����Ⱥ꣬ShaderType::NONE=-1
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
unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {//����shader�����ͺ�Դ���룬����shader��id
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();//��Դ����ת��Ϊָ��,c_str()�Ǹ�ָ��source��ָ��
    glShaderSource(id, 1, &src, nullptr);//��Դ���봫��shader,idΪshader��id��1��ʾֻ��һ��Դ���룬&srcΪԴ�����ָ��ĵ�ַ��nullptr��ʾû�и�����Ϣ
    glCompileShader(id);//����shader 

    int result;
#ifdef _DEBUG
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);//��ȡ����״̬
    if (result == GL_FALSE) {//����ʧ��
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);//��ȡ������Ϣ����,&length��ʾҪ��ȡ����Ϣ�ĳ���
        char* message = (char*)alloca(length * sizeof(char));//�����ڴ�
        glGetShaderInfoLog(id, length, &length, message);//��ȡ������Ϣ���ѱ�����Ϣ����message
        std::cout << "Fail to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << "shader " << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
#endif
    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {//����shader,���붥��shader��Ƭ��shader
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);//����һ������shader��GL_VERTEX_SHADER��ʾ����
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);//����һ��Ƭ��shader,GL_FRAGMENT_SHADER��ʾ����

    glAttachShader(program, vs);//�Ѷ���shader��program���ӵ�һ��
    glAttachShader(program, fs);//��Ƭ��shader��program���ӵ�һ��

    glLinkProgram(program);

    glDeleteShader(vs);//ɾ������shader,��Ϊ�Ѿ���program���ӣ����Կ���ɾ��
    glDeleteShader(fs);//ɾ��Ƭ��shader,��Ϊ�Ѿ���program���ӣ����Կ���ɾ��

    return program; //����program��id
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
