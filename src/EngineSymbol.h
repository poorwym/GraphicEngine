#pragma once

#include "Camera.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"

class EngineSymbol {
private:
    VertexArray* m_VAO;
    VertexBuffer* m_VBO;
    Shader* m_Shader;
public:
    EngineSymbol();
    void Render(Camera& camera);
};