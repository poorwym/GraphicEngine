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

// [0.0, 1.0] �����
float frandom();
// [-1.0, 1.0] �����
float sfrandom();

// ��ʼ��ÿ�����ӵ�IBO
IndexBuffer* CreateElementArrayBufferObject(int numParticles);

struct ParticleVertex {
	glm::vec4 position;
	glm::vec2 texcoord;
	float factor;
};

struct alignas(16) Particle {
	// ����λ��
	glm::vec4 position;
	// ���ӷ�����λ��
	glm::vec4 emitter_position;
	// �������ĵ���ɢ�Ķ���λ�ã���������õ㾫���2D����Ͳ���Ҫ�ˣ�Ч������ã�
	glm::vec4 vertex[4];
	// �����ٶ�
	glm::vec4 velocity;
	// ���ӵ�ǰ����
	float life;
	// ��������
	float maxLife;
	// ��������˥�����ӣ����ڿ������Ӵ�С��͸����
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
	// SSBO�󶨵Ľṹ��
	std::vector<Particle> particles;
	std::vector<Particle> emitters;
	// �������ĵ���ɢ�Ķ���λ�ã���������õ㾫���2D����Ͳ���Ҫ�ˣ�Ч������ã�
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

	// ��������״̬
	virtual void Update(float deltaTime) {}

	// ��������
	virtual void Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
};

class FlameParticleSystem : public ParticleSystem
{
private:
	// Ӱ�����ӷֲ�������
	int n;
	int adj_value;
	// �������������Χ
	float max_life;
	float min_life;
	// ��������ٶȷ�Χ
	float max_velocity;
	float min_velocity;
	// ��������˥����Χ��С�ڸð뾶���������������������ģ�
	float radius;
public:
	FlameParticleSystem(int numParticles, Shader* renderProgram, ComputeShader* computeShader, int n, int adj_value, float max_life, float min_life, float max_velocity, float min_velocity, float radius)
		: ParticleSystem(numParticles, renderProgram, computeShader), n(n), adj_value(adj_value), max_life(max_life), min_life(min_life), max_velocity(max_velocity), min_velocity(min_velocity), radius(radius) {
		InitParticles();
	}

	void InitParticles();

	// ��������״̬
	void Update(float deltaTime);
};
