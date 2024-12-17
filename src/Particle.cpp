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

// 初始化每个粒子的IBO（直接在中心点绑定纹理则不需要）
IndexBuffer* CreateElementArrayBufferObject(int numParticles) {
	unsigned int* indexes = new unsigned int[numParticles * 6];
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
	IndexBuffer* ibo = new IndexBuffer(indexes, numParticles * 6);
	return ibo;
}

// 绘制粒子
void ParticleSystem::Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
	renderProgram->Bind();
	renderProgram->SetUniformMat4f("Model", model);
	renderProgram->SetUniformMat4f("View", view);
	renderProgram->SetUniformMat4f("Projection", projection);
	emitterVAO->Bind();
	particleIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, numParticles * 6, GL_UNSIGNED_INT, 0));
	emitterVAO->Unbind();
	particleIBO->Unbind();

	particleVAO->Bind();
	particleIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, numParticles * 6, GL_UNSIGNED_INT, 0));
	particleVAO->Unbind();
	particleIBO->Unbind();
	renderProgram->Unbind();
}

void FlameParticleSystem::InitParticles() {
	// 初始化粒子状态
	for (auto& p : particles) {
		// 初始化粒子位置
		float x = 0.0f, z = 0.0f;
		for (int i = 0; i < n; i++) {
			x += (sfrandom() * adj_value);
			z += (sfrandom() * adj_value);
		}
		p.emitter_position = glm::vec4(x, 0.0, z, 1.0);
		p.position = p.emitter_position;
		// 初始化粒子速度
		float InitVeloc = (max_velocity - min_velocity) * frandom() + min_velocity;
		p.velocity = glm::vec4(0.0, InitVeloc, 0.0, 0.0);
		// 初始化粒子寿命
		float dist = sqrt(p.position.x * p.position.x + p.position.z * p.position.z);
		float InitLife = (max_life - min_life) * frandom() + min_life;
		if (dist <= radius) p.maxLife = InitLife * 1.3;
		else p.maxLife = InitLife;
		p.life = p.maxLife;
		// 初始化粒子顶点位置（绑定纹理则不需要）
		for (int i = 0; i < 4; i++) {
			glm::vec2 texcoord = glm::vec2(((i - 1) & 2) >> 1, (i & 2) >> 1);
			float spriteSize = 0.5;
			glm::vec4 vertex = p.position + glm::vec4(glm::vec2(texcoord.x * 2.0 - 1.0, texcoord.y * 2.0 - 1.0) * spriteSize, 0.0, 1.0);
			particleVertex.push_back({ vertex, texcoord, 1.0 });
			emitterVertex.push_back({ vertex, texcoord, 1.0 });
		}
	}
	// 初始化发射器状态
	std::memcpy(emitters.data(), particles.data(), sizeof(Particle) * numParticles);
	// 初始化 SSBO
	particleBuffer = new ShaderStorageBuffer(particles.data(), particles.size() * sizeof(Particle), 0);
	emitterBuffer = new ShaderStorageBuffer(emitters.data(), emitters.size() * sizeof(Particle), 1);
	// 初始化 VAO VBO IBO
	VertexBufferLayout layout;
	layout.Push<float>(4); // position: vec4
	layout.Push<float>(2); // texcoord: vec2
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
	particleIBO = CreateElementArrayBufferObject(numParticles);
}

// 更新粒子状态 
void FlameParticleSystem::Update(float deltaTime) {
	computeShader->Bind();
	computeShader->SetUniform1f("deltaTime", deltaTime);
	computeShader->SetUniform1f("max_velocity", max_velocity);
	computeShader->SetUniform1f("min_velocity", min_velocity);

	particleBuffer->Bind();
	emitterBuffer->Bind();
	computeShader->Dispatch((numParticles + 511) / 512, 1, 1);
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
		for (int j = 0; j < 4; j++) {
			particleVertex[i * 4 + j].position = temp[i].vertex[j];
			particleVertex[i * 4 + j].factor = temp[i].factor;
		}
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
		for (int j = 0; j < 4; j++) {
			emitterVertex[i * 4 + j].position = temp[i].vertex[j];
			emitterVertex[i * 4 + j].factor = temp[i].factor;
		}
	}
	emitterVBO->Bind();
	emitterVBO->setData(emitterVertex.data(), emitterVertex.size() * sizeof(ParticleVertex));
	emitterVBO->Unbind();
}