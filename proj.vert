#version 150 core

in vec2 position;
in vec3 color;
in vec2 texcoord;

out vec3 Color;
out vec2 Texcoord;

uniform mat4 trans;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 flip;

void main()
{
	mat3 scale_f = mat3(0.5);
	mat4 scale = mat4(scale_f); 
	Color = color;
	Texcoord = texcoord;
	gl_Position = proj * view * trans * flip * scale * vec4(position.x, position.y, 0.0, 1.0);
}

