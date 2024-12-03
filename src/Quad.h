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

class SimpleQuad {
public:
	SimpleQuad();
	SimpleQuad(const std::vector<float>& vertices);
	~SimpleQuad();

	void Render(Shader& shader);

private:
	VertexArray* m_VAO;
	VertexBuffer* m_VBO;
};

class TileQuad {
private:
	std::vector<std::vector<float>> m_Vertices;
	std::vector<SimpleQuad*> m_Quads;

	VertexArray* m_VAO;
	VertexBuffer* m_VBO;
public:
	TileQuad(int n);
	~TileQuad();

	void Render(Shader& shader);
};