#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Shader.h"

class Quad {
public:
	Quad();
	~Quad();
	
	void Render(Shader& shader);

private:
	VertexArray* m_VAO;
	VertexBuffer* m_VBO;
	IndexBuffer* m_IBO;
};