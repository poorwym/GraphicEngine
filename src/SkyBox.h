#pragma once
#include <vector>
#include <string>
#include <GL/glew.h>
#include "Shader.h" 
#include "Renderer.h" 

class Skybox
{
public:
    Skybox(const std::vector<std::string>& faces);
    Skybox(std::string name);
    ~Skybox();
    void Bind(unsigned int slot);
    void Unbind();
    void Draw(Camera& camera);

private:
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int cubemapTexture;
    Shader skyboxShader;

    unsigned int loadCubemap(const std::vector<std::string>& faces);
};