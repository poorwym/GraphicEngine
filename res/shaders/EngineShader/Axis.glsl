#shader vertex

#version 450 core


layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

uniform mat4 u_ProjectionView;

void main(){
    gl_Position = u_ProjectionView * vec4(a_Position, 1.0);
}

#shader fragment

#version 450 core

out vec4 FragColor;

layout(location = 1) out vec3 a_Color;

void main(){
    FragColor = vec4(a_Color, 1.0);
}