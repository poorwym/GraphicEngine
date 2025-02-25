#pragma once

#include <GL/glew.h>

#include "VertexArray.h"

#include "IndexBuffer.h"

#include "Shader.h"
#include "Camera.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Camera* camera, Shader& shader, const glm::mat4& model) const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, Shader& shader) const;
    void Clear() const;
};