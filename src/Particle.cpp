#include "Particle.h"
#include <iostream>

// [0.0, 1.0] 随机数
float frandom() {
	return (float)rand() / (float)RAND_MAX;
}

// [-1.0, 1.0] 随机数
float sfrandom() {
	return frandom() * 2.0 - 1.0;
}

// 绘制粒子
void ParticleSystem::Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
	renderProgram->Bind();
	renderProgram->SetUniformMat4f("Model", model);
	renderProgram->SetUniformMat4f("View", view);
	renderProgram->SetUniformMat4f("Projection", projection);
	emitterVAO->Bind();
	GLCall(glDrawArrays(GL_POINTS, 0, numParticles));
	emitterVAO->Unbind();

	particleVAO->Bind();
	GLCall(glDrawArrays(GL_POINTS, 0, numParticles));
	particleVAO->Unbind();
	renderProgram->Unbind();
}

void FlameParticleSystem::InitParticles() {
	// 纹理
	//texture1 = new Texture("res/Particles/flame.png");
	// 初始化粒子状态
	for (auto& e : emitters) {
		// 初始化发射器位置
		InitParticlePosition(e);
		// 初始化发射器速度
		InitParticleVelocity(e);
		// 初始化发射器寿命
		InitParticleLife(e);
		// VAO数据
		emitterVertex.push_back({ e.position, 1.0 });
	}
	for (auto& p : particles) {
		// 初始化粒子位置
		InitParticlePosition(p);
		// 初始化粒子速度
		InitParticleVelocity(p);
		// 初始化粒子寿命
		InitParticleLife(p);
		// VAO数据
		particleVertex.push_back({ p.position, 1.0 });
	}
	// 初始化 SSBO
	particleBuffer = new ShaderStorageBuffer(particles.data(), particles.size() * sizeof(Particle), 2);
	emitterBuffer = new ShaderStorageBuffer(emitters.data(), emitters.size() * sizeof(Particle), 3);
	// 初始化 VAO VBO IBO
	VertexBufferLayout layout;
	layout.Push<float>(4); // position: vec4
	layout.Push<float>(1); // factor: float
	particleVBO->Bind();
	particleVBO->setData(particleVertex.data(), particleVertex.size() * sizeof(ParticleVertex));
	particleVBO->Unbind();
	particleVAO->Bind();
	particleVAO->AddBuffer(*particleVBO, layout);
	particleVAO->Unbind();

	emitterVBO->Bind();
	emitterVBO->setData(emitterVertex.data(), emitterVertex.size() * sizeof(ParticleVertex));
	emitterVBO->Unbind();
	emitterVAO->Bind();
	emitterVAO->AddBuffer(*emitterVBO, layout);
	emitterVAO->Unbind();
}

void FlameParticleSystem::InitParticlePosition(Particle& p) {
	float x = 0.0f, z = 0.0f;
	for (int i = 0; i < n; i++) {
		x += (sfrandom() * adj_value);
		z += (sfrandom() * adj_value);
	}
	if (abs(x) >= (4.0 * n / 5.0) * adj_value) x = sfrandom();
	if (abs(z) >= (4.0 * n / 5.0) * adj_value) z = sfrandom();
	p.emitter_position = glm::vec4(x, 0.0, z, 1.0);
	p.position = p.emitter_position;
}

void FlameParticleSystem::InitParticleVelocity(Particle& p) {
	float InitVeloc = (max_velocity - min_velocity) * frandom() + min_velocity;
	p.velocity = glm::vec4(0.0, InitVeloc, 0.0, 0.0);
}

void FlameParticleSystem::InitParticleLife(Particle& p) {
	float dist = sqrt(p.position.x * p.position.x + p.position.z * p.position.z);
	float InitLife = (max_life - min_life) * frandom() + min_life;
	if (dist >= radius) p.maxLife = InitLife * 0.6;
	else p.maxLife = InitLife * 1.2;
	p.life = p.maxLife;
}

// 更新粒子状态 
void FlameParticleSystem::Update(float deltaTime) {
	computeShader->Bind();
	computeShader->SetUniform1f("deltaTime", deltaTime);
	computeShader->SetUniform1f("max_velocity", max_velocity);
	computeShader->SetUniform1f("min_velocity", min_velocity);

	particleBuffer->Bind();
	emitterBuffer->Bind();
	computeShader->Dispatch((numParticles + 255) / 256, 1, 1);
	particleBuffer->Unbind();
	emitterBuffer->Unbind();

	computeShader->Unbind();

	std::vector<Particle> temp;
	temp.resize(numParticles);
	void* mappedData = particleBuffer->Map(GL_READ_ONLY);
	if (mappedData) {
		std::memcpy(temp.data(), mappedData, sizeof(Particle) * numParticles);
		particleBuffer->Unmap();
	}
	for (int i = 0; i < particles.size(); i++) {
		particleVertex[i].position = temp[i].position;
		particleVertex[i].factor = temp[i].factor;
	}
	particleVBO->Bind();
	particleVBO->setData(particleVertex.data(), particleVertex.size() * sizeof(ParticleVertex));
	particleVBO->Unbind();

	mappedData = emitterBuffer->Map(GL_READ_ONLY);
	if (mappedData) {
		std::memcpy(temp.data(), mappedData, sizeof(Particle) * numParticles);
		emitterBuffer->Unmap();
	}
	for (int i = 0; i < emitters.size(); i++) {
		emitterVertex[i].position = temp[i].position;
		emitterVertex[i].factor = temp[i].factor;
	}
	emitterVBO->Bind();
	emitterVBO->setData(emitterVertex.data(), emitterVertex.size() * sizeof(ParticleVertex));
	emitterVBO->Unbind();
}