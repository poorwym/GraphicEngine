#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "ShaderStorageBuffer.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "Renderer.h"

// [0.0, 1.0] 随机数
float frandom();
// [-1.0, 1.0] 随机数
float sfrandom();

// 初始化每个粒子的IBO
IndexBuffer* CreateElementArrayBufferObject(int numParticles);

struct ParticleVertex {
	glm::vec4 position;
	glm::vec2 texcoord;
	float factor;
};

struct alignas(16) Particle {
	// 粒子位置
	glm::vec4 position;
	// 粒子发射器位置
	glm::vec4 emitter_position;
	// 粒子中心点扩散的顶点位置（如果可以用点精灵绑定2D纹理就不需要了，效果会更好）
	glm::vec4 vertex[4];
	// 粒子速度
	glm::vec4 velocity;
	// 粒子当前年龄
	float life;
	// 粒子寿命
	float maxLife;
	// 粒子年龄衰减因子，用于控制粒子大小和透明度
	float factor;
};

class ParticleSystem {
protected:
	// SSBO
	ShaderStorageBuffer* particleBuffer;
	ShaderStorageBuffer* emitterBuffer;
	VertexArray* particleVAO;
	VertexArray* emitterVAO;
	VertexBuffer* particleVBO;
	VertexBuffer* emitterVBO;
	IndexBuffer* particleIBO;
	// SSBO绑定的结构体
	std::vector<Particle> particles;
	std::vector<Particle> emitters;
	// 粒子中心点扩散的顶点位置（如果可以用点精灵绑定2D纹理就不需要了，效果会更好）
	std::vector<ParticleVertex> particleVertex;
	std::vector<ParticleVertex> emitterVertex;
	ComputeShader* computeShader;
	Shader* renderProgram;
	int numParticles;
public:
	ParticleSystem(int numParticles, Shader* renderProgram, ComputeShader* computeShader)
		: numParticles(numParticles), renderProgram(renderProgram), computeShader(computeShader) {
		particles.resize(numParticles);
		emitters.resize(numParticles);
		particleVAO = new VertexArray();
		emitterVAO = new VertexArray();
		particleVBO = new VertexBuffer(nullptr, numParticles * 4 * sizeof(ParticleVertex));
		emitterVBO = new VertexBuffer(nullptr, numParticles * 4 * sizeof(ParticleVertex));
	}

	virtual void InitParticles() {}

	// 更新粒子状态
	virtual void Update(float deltaTime) {}

	// 绘制粒子
	virtual void Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
};

class FlameParticleSystem : public ParticleSystem
{
private:
	// 影响粒子分布的因子
	int n;
	int adj_value;
	// 粒子随机寿命范围
	float max_life;
	float min_life;
	// 粒子随机速度范围
	float max_velocity;
	float min_velocity;
	// 粒子寿命衰减范围，小于该半径的寿命更长（更靠近中心）
	float radius;
public:
	FlameParticleSystem(int numParticles, Shader* renderProgram, ComputeShader* computeShader, int n, int adj_value, float max_life, float min_life, float max_velocity, float min_velocity, float radius)
		: ParticleSystem(numParticles, renderProgram, computeShader), n(n), adj_value(adj_value), max_life(max_life), min_life(min_life), max_velocity(max_velocity), min_velocity(min_velocity), radius(radius) {
		InitParticles();
	}

	void InitParticles();

	// 更新粒子状态
	void Update(float deltaTime);
};
