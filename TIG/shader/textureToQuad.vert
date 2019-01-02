#version 400 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoord;

out vec2 xTexCoord;

void main()
{
	gl_Position = vec4(inPos.x, inPos.y, 0.0f, 1.0f);
	xTexCoord = inTexCoord;
}