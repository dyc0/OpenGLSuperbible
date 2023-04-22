#version 150 core

in vec2 Texcoord

out vec4 outColor

uniform sampler2D texFrameBuffer;

void main()
{
	outColor = texture(texFrameBuffer, Texcoord);
}
