 #shader vertex

 #version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_MVP;

out vec3 Normal;
out vec2 TexCoords;

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1.0);
    Normal = a_Normal;
    TexCoords = a_TexCoords;
}

 #shader fragment

 #version 330 core

in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_Texture, TexCoords) ;
}      