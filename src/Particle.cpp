#include "Particle.h"
#include <iostream>

// [0.0, 1.0] �����
float frandom() {
	return (float)rand() / (float)RAND_MAX;
}

// [-1.0, 1.0] �����
float sfrandom() {
	return frandom() * 2.0 - 1.0;
}

GLuint LoadTexture(const std::string& path) {
	GLuint texture;
	// ʹ�� OpenCV ����ͼ��
	cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED); // ����ͼ�񣨰��� alpha ͨ����
	if (image.empty()) {
		std::cerr << "Failed to load texture at path: " << path << std::endl;
	}
	else {
		std::cout << "Loaded texture at path: " << path << std::endl;
	}

	// OpenCV ���ص�ͼ������ת��Ϊ OpenGL ���õĸ�ʽ
	int width = image.cols;
	int height = image.rows;
	int BPP = image.channels(); // ͨ������RGBA = 4

	// OpenCV ��ȡ��ͼ�������Ѿ�����ȷ��ʽ��ֱ��ʹ����
	unsigned char* LocalBuffer = image.data;

	// ���� OpenGL ����
	GLCall(glGenTextures(1, &texture));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture));

	// �����������
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// �ϴ��������ݵ� GPU
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	return texture;
}

// ��������
void FlameParticleSystem::Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
	renderProgram->Bind();
	renderProgram->SetUniformMat4f("Model", model);
	renderProgram->SetUniformMat4f("View", view);
	renderProgram->SetUniformMat4f("Projection", projection);
	emitterVAO->Bind();
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture1));
	GLCall(glDrawArrays(GL_POINTS, 0, numParticles));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	emitterVAO->Unbind();

	particleVAO->Bind();
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture1));
	GLCall(glDrawArrays(GL_POINTS, 0, numParticles));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	particleVAO->Unbind();
	renderProgram->Unbind();
}

void FlameParticleSystem::InitParticles() {
	// ����
	texture1 = LoadTexture("res/Particles/PNG/fire_01.png");
	// ��ʼ������״̬
	for (auto& e : emitters) {
		// ��ʼ��������λ��
		InitParticlePosition(e);
		// ��ʼ���������ٶ�
		InitParticleVelocity(e);
		// ��ʼ������������
		InitParticleLife(e);
		// VAO����
		emitterVertex.push_back({ e.position, 1.0 });
	}
	for (auto& p : particles) {
		// ��ʼ������λ��
		InitParticlePosition(p);
		// ��ʼ�������ٶ�
		InitParticleVelocity(p);
		// ��ʼ����������
		InitParticleLife(p);
		// VAO����
		particleVertex.push_back({ p.position, 1.0 });
	}
	// ��ʼ�� SSBO
	particleBuffer = new ShaderStorageBuffer(particles.data(), particles.size() * sizeof(Particle), 2);
	emitterBuffer = new ShaderStorageBuffer(emitters.data(), emitters.size() * sizeof(Particle), 3);
	// ��ʼ�� VAO VBO IBO
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
	p.emitter_position = glm::vec4(x * 0.7, 0.0, z * 0.7, 1.0);
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

// ��������״̬ 
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