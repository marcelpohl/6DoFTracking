#version 400 core
in vec2 xTexCoord;

out vec4 FragColor;

uniform sampler2D backgroundColorTexture;
uniform sampler2D backgroundDepthTexture;

void main()
{
	vec3 bgColor = texture(backgroundColorTexture, xTexCoord).rgb;
	float bgDepth = texture(backgroundDepthTexture, xTexCoord).r;
	
	FragColor = vec4(bgColor, bgDepth);
} 