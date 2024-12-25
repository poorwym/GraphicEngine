#version 450 core

layout(local_size_x = 512) in;

struct Particle {
	vec4 position;
	vec4 emitter_position;
	vec4 vertex[4];
	vec4 velocity;
	float life;
	float maxLife;
	float factor;
};

layout(std430, binding = 0) buffer ParticleBuffer {
	Particle particles[];
};

layout(std430, binding = 1) buffer EmitterBuffer {
	Particle emitters[];
};

// ����֡ʱ��
uniform float deltaTime;
// ���������С���ٶ�
uniform float max_velocity;
uniform float min_velocity;

float random(float seed) {
    return fract(sin(seed) * 43758.5453123);
}

void main() {
	uint id = gl_GlobalInvocationID.x;

	Particle p = particles[id];
	Particle e = emitters[id];

	// ������������������״̬
	if (p.life <= 0.0) 
	{
		p.velocity = vec4(0.0, (max_velocity - min_velocity) * random(id) + min_velocity, 0.0, 0.0);
		p.position = p.emitter_position;
		p.life = p.maxLife;
	} 
	// �����������״̬
	else
	{
		p.velocity += deltaTime * vec4(0.0, 1.0, 0.0, 0.0);
		p.position += deltaTime * p.velocity;
		p.life -= deltaTime;
	}

	// ���·�����״̬
	if (e.life <= 0.0)
	{
		e.life = e.maxLife;
	}
	else
	{
		e.life -= deltaTime;
	}

	// ���� factor��Ӱ�����ӵĴ�С��͸����
	p.factor = 1.0 / ((p.life - p.maxLife / 2) * (p.life - p.maxLife / 2) + 1.0);
	e.factor = 1.0 / ((e.life - e.maxLife / 2) * (e.life - e.maxLife / 2) + 1.0);

	for (int i = 0; i < 4; i++) {
		vec2 texcoord = vec2(((i - 1) & 2) >> 1, (i & 2) >> 1);
		float spriteSize = 0.5 * p.factor;
		p.vertex[i] = p.position + vec4(vec2(texcoord.x * 2.0 - 1.0, texcoord.y * 2.0 - 1.0) * spriteSize, 0.0, 1.0);
		e.vertex[i] = e.position + vec4(vec2(texcoord.x * 2.0 - 1.0, texcoord.y * 2.0 - 1.0) * spriteSize, 0.0, 1.0);
	}

	particles[id] = p;
	emitters[id] = e;
}