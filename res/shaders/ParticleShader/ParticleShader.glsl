#shader vertex
#version 450 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in float factor;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 VOUT_Texcoord;
out float VOUT_Factor;

void main()
{
	VOUT_Texcoord = texcoord;
	VOUT_Factor = factor;
	gl_Position = Projection * View * Model * aPos;
}

#shader fragment
#version 450 core

in vec2 VOUT_Texcoord;
in float VOUT_Factor;

out vec4 FragColor;

void main()
{
	// 计算渐变透明度
	float distance = length(VOUT_Texcoord * 2.0 - vec2(1.0)) * 3.0;
	float alpha = exp(-distance * distance);
	if (alpha < 0.15) discard;
	FragColor = vec4(1.0f, 0.8f, 0.05f, 1.0f);
}