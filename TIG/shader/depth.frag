#version 400 core
in vec2 xTexCoord;

out vec4 FragColor;

uniform sampler2D backgroundColorTexture;
uniform sampler2D backgroundDepthTexture;

uniform float near;
uniform float far;

float depthToZ(float depth) {
	return ((near * far) / (far - depth * (far - near)) );
}

void main()
{
	vec3 bgColor = texture(backgroundColorTexture, xTexCoord).xyz;
	float bgDepth = texture(backgroundDepthTexture, xTexCoord).x * 2;
	//float objZ = depthToZ(objDepth) * 0.2;
	
	FragColor = vec4(bgColor, 1.0f);
	gl_FragDepth = bgDepth;
} 