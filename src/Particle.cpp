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

GLuint LoadTexture(const std::string& path) {
	GLuint texture;
	// 使用 OpenCV 加载图像
	cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED); // 加载图像（包括 alpha 通道）
	if (image.empty()) {
		std::cerr << "Failed to load texture at path: " << path << std::endl;
	}
	else {
		std::cout << "Loaded texture at path: " << path << std::endl;
	}

	// OpenCV 加载的图像数据转换为 OpenGL 可用的格式
	int width = image.cols;
	int height = image.rows;
	int BPP = image.channels(); // 通道数，RGBA = 4

	// OpenCV 读取的图像数据已经是正确格式，直接使用它
	unsigned char* LocalBuffer = image.data;

	// 创建 OpenGL 纹理
	GLCall(glGenTextures(1, &texture));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture));

	// 设置纹理参数
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// 上传纹理数据到 GPU
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	return texture;
}

// 绘制粒子
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
	// 纹理
	texture1 = LoadTexture("res/Particles/PNG/fire_01.png");
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