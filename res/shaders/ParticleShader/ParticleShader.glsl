#shader vertex
#version 450 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in float factor;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 VOUT_Texcoord;
out float VOUT_Factor;

void main()
{
	VOUT_Factor = factor;
	gl_Position = Projection * View * Model * aPos;
	gl_PointSize = 20 * factor;
}

#shader fragment
#version 450 core

in float VOUT_Factor;

uniform sampler2D texture1;

out vec4 FragColor;

void main()
{
	// vec4 texture_color = texture(texture1, gl_PointCoord);
	// if (texture_color.r < 0.1) discard;
	// FragColor = vec4(texture_color.rgb, VOUT_Factor);

	vec4 color = vec4(1.0, 0.8, 0.05, VOUT_Factor);
	vec2 temp = gl_PointCoord - vec2(0.5);
	float f = dot(temp, temp);
	if (f > 0.25) discard;
	FragColor = color;
}