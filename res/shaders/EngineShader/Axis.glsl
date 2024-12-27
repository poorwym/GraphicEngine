#shader vertex

#version 450 core


layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

uniform mat4 u_ProjectionView;

out vec3 v_Color;

void main(){
    gl_Position = u_ProjectionView * vec4(a_Position, 1.0);
    v_Color = a_Color;
}

#shader fragment

#version 450 core
in vec3 v_Color;

out vec4 FragColor;


void main(){
    FragColor = vec4(v_Color, 1.0);
}