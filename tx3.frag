#version 150 core

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texKitten;
uniform sampler2D texPuppy;
uniform float time;

void main()
{
	float factor = (sin(time*3.0) + 1.0) / 2.0;
	vec2 HalfCoord = vec2(Texcoord.x, Texcoord.y * 2.0f);
	if(Texcoord.y < 0.5)
		outColor = mix(texture(texKitten, HalfCoord), texture(texPuppy, Texcoord), factor);
	else
		outColor = mix(texture(texKitten, vec2(HalfCoord.x + sin(HalfCoord.y * 60.0f + time * 2.0f) / 100.0f, -HalfCoord.y)) * vec4(0.7, 0.7, 1.0, 1.0), texture(texPuppy, Texcoord), factor * 0.5);
}
