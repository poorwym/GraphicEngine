// Skybox.cpp
#include "Skybox.h"
#include <stb_image/stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ������Ķ�������
float skyboxVertices[] = {
    // positions          
    -100.0f,  100.0f, -100.0f,
    -100.0f, -100.0f, -100.0f,
     100.0f, -100.0f, -100.0f,
     100.0f, -100.0f, -100.0f,
     100.0f,  100.0f, -100.0f,
    -100.0f,  100.0f, -100.0f,

    -100.0f, -100.0f,  100.0f,
    -100.0f, -100.0f, -100.0f,
    -100.0f,  100.0f, -100.0f,
    -100.0f,  100.0f, -100.0f,
    -100.0f,  100.0f,  100.0f,
    -100.0f, -100.0f,  100.0f,

     100.0f, -100.0f, -100.0f,
     100.0f, -100.0f,  100.0f,
     100.0f,  100.0f,  100.0f,
     100.0f,  100.0f,  100.0f,
     100.0f,  100.0f, -100.0f,
     100.0f, -100.0f, -100.0f,

    -100.0f, -100.0f,  100.0f,
    -100.0f,  100.0f,  100.0f,
     100.0f,  100.0f,  100.0f,
     100.0f,  100.0f,  100.0f,
     100.0f, -100.0f,  100.0f,
    -100.0f, -100.0f,  100.0f,

    -100.0f,  100.0f, -100.0f,
     100.0f,  100.0f, -100.0f,
     100.0f,  100.0f,  100.0f,
     100.0f,  100.0f,  100.0f,
    -100.0f,  100.0f,  100.0f,
    -100.0f,  100.0f, -100.0f,

    -100.0f, -100.0f, -100.0f,
    -100.0f, -100.0f,  100.0f,
     100.0f, -100.0f, -100.0f,
     100.0f, -100.0f, -100.0f,
    -100.0f, -100.0f,  100.0f,
     100.0f, -100.0f,  100.0f
};

Skybox::Skybox(const std::vector<std::string>& faces)
    : skyboxShader("res/shaders/skybox.shader") // ȷ�� Shader ����Խ��ܶ����Ƭ����ɫ��·��
{
    // ������������ͼ
    cubemapTexture = loadCubemap(faces);

    // �����������VAO��VBO
    GLCall(glGenVertexArrays(1, &skyboxVAO));
    GLCall(glGenBuffers(1, &skyboxVBO));
    GLCall(glBindVertexArray(skyboxVAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GLCall(glBindVertexArray(0));

    // ������ɫ��
    skyboxShader.Bind();
    skyboxShader.setUniform1i("skybox", 0);
}

Skybox::~Skybox()
{
    GLCall(glDeleteVertexArrays(1, &skyboxVAO));
    GLCall(glDeleteBuffers(1, &skyboxVBO));
    GLCall(glDeleteTextures(1, &cubemapTexture));
}

void Skybox::Draw(Camera& camera)
{
    // ������ͼ����ʹ��պв��������λ��Ӱ��
    skyboxShader.Bind();
    skyboxShader.setUniformMat4f("view", camera.GetViewMatrix());
    skyboxShader.setUniformMat4f("projection", camera.GetProjectionMatrix());
    skyboxShader.setUniformMat4f("model", glm::translate(glm::mat4(1.0f), camera.GetPosition()));

    // ��Ⱦ��պ�
    GLCall(glDepthFunc(GL_LEQUAL));  // ������Ⱥ�����ȷ����պ���Ⱦ��������������֮��
    GLCall(glBindVertexArray(skyboxVAO));
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
    GLCall(glBindVertexArray(0));
    GLCall(glDepthFunc(GL_LESS)); // �ָ�Ĭ����Ⱥ���
}

unsigned int Skybox::loadCubemap(const std::vector<std::string>& faces)
{
    unsigned int textureID;
    GLCall(glGenTextures(1, &textureID));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));
   

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // ͨ����պв���Ҫ��ת

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 4);
        if (data)
        {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
            stbi_image_free(data);
        }
        else
        {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // ������������ͼ�Ĳ���
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    // ȷ�������з����϶������ظ�
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    return textureID;
}