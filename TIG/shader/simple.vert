#version 400 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec2 xTexCoord;
out vec3 xNormal;
out vec3 xWorldPos;
out float xFragDepth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(inPos, 1.0);
	
	xFragDepth = gl_Position.z;
	xTexCoord = inTexCoord;
	xWorldPos = vec3(model * vec4(inPos, 1.0));
	xNormal = mat3(transpose(inverse(model))) * inNormal;
}