// Skybox.cpp
#include "Skybox.h"
#include <stb_image/stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 立方体的顶点数据
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
    : skyboxShader("res/shaders/skybox.shader") // 确保 Shader 类可以接受顶点和片段着色器路径
{
    // 加载立方体贴图
    cubemapTexture = loadCubemap(faces);

    // 设置立方体的VAO和VBO
    GLCall(glGenVertexArrays(1, &skyboxVAO));
    GLCall(glGenBuffers(1, &skyboxVBO));
    GLCall(glBindVertexArray(skyboxVAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GLCall(glBindVertexArray(0));

    // 配置着色器
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
    // 更改视图矩阵，使天空盒不受摄像机位置影响
    skyboxShader.Bind();
    skyboxShader.setUniformMat4f("view", camera.GetViewMatrix());
    skyboxShader.setUniformMat4f("projection", camera.GetProjectionMatrix());
    skyboxShader.setUniformMat4f("model", glm::translate(glm::mat4(1.0f), camera.GetPosition()));

    // 渲染天空盒
    GLCall(glDepthFunc(GL_LEQUAL));  // 更改深度函数以确保天空盒渲染在其他所有物体之后
    GLCall(glBindVertexArray(skyboxVAO));
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
    GLCall(glBindVertexArray(0));
    GLCall(glDepthFunc(GL_LESS)); // 恢复默认深度函数
}

unsigned int Skybox::loadCubemap(const std::vector<std::string>& faces)
{
    unsigned int textureID;
    GLCall(glGenTextures(1, &textureID));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));
   

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // 通常天空盒不需要翻转

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

    // 设置立方体贴图的参数
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    // 确保在所有方向上都不会重复
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    return textureID;
}