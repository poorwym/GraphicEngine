#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>
#include "Shader.h"

float frandom() {
	return (float)rand() / (float)RAND_MAX;
}

float sfrandom() {
	return frandom() * 2.0 - 1.0;
}

GLuint CreateElementArrayBufferObject(int numParticles) {
	unsigned int *indexes = new unsigned int[numParticles * 6];
	unsigned int* temp = indexes;
	for (size_t i = 0; i < numParticles; i++) {
		unsigned int index = unsigned int(i << 2);
		*temp++ = index;
		*temp++ = index + 1;
		*temp++ = index + 2;
		*temp++ = index;
		*temp++ = index + 2;
		*temp++ = index + 3;
	}
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numParticles * 6, indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;
}

struct Particle {
	glm::vec4 position;
	/*glm::vec3 velocity;
	float life;
	float age;*/
};

class ParticleSystem {
protected:
	GLuint particleBuffer;
	GLuint particleIBO;
	std::vector<Particle> particles;
	//GLuint computeShader;
	Shader* renderProgram;
	int numParticles;
public:
	ParticleSystem(int numParticles, Shader* renderProgram)
		: numParticles(numParticles), renderProgram(renderProgram) {
		particles.resize(numParticles);
	}

	virtual ~ParticleSystem() {
		glDeleteBuffers(1, &particleBuffer);
	}

	virtual void InitParticles() {}

	virtual void Render() {
		renderProgram->Bind();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particleIBO);
		glDrawElements(GL_TRIANGLES, numParticles * 6, GL_UNSIGNED_INT, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		renderProgram->Unbind();
	}
};

class FlameParticleSystem : public ParticleSystem
{
private:
	int n;
	int adj_value;
	float size;
	float alpha;
public:
	FlameParticleSystem(int numParticles, Shader* renderProgram, int n, int adj_value)
		: ParticleSystem(numParticles, renderProgram), n(n), adj_value(adj_value) {
		InitParticles();
	}

	void InitParticles() {
		for (auto& p : particles) {
			float x = 0.0f, z = 0.0f;
			for (int i = 0; i < n; i++) {
				x += (sfrandom() * adj_value);
				z += (sfrandom() * adj_value);
			}
			p.position = glm::vec4(x, 0.0, z, 1.0);
		}
		glGenBuffers(1, &particleBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		particleIBO = CreateElementArrayBufferObject(numParticles);
	}
};
