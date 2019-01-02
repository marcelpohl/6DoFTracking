#version 400 core
in vec2 xTexCoord;

uniform sampler2D screenTexture;

out vec4 FragColor;

void main()
{
	vec4 color = texture(screenTexture, xTexCoord);
	FragColor = color;
} 