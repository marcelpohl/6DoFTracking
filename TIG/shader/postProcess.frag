#version 400 core
in vec2 xTexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D noiseTexture;

uniform int radius;
uniform int width;
uniform int height;

void main()
{
	// apply mean filter
	float offsetX = 1.0f / width;
	float offsetY = 1.0f / height;

	int pixelCount = 0;
	vec3 color = vec3(0.0f, 0.0f, 0.0f);
	for (int x = -radius; x <= radius; x++) {
		for (int y = -radius; y <= radius; y++) {
			vec2 coords = vec2(xTexCoord.x + x * offsetX, xTexCoord.y + y * offsetY);
			color += texture(screenTexture, coords).xyz;
			pixelCount++;
		}
	}
	color /= pixelCount;
	
	// add noise
	float gaussVal = texture(noiseTexture, xTexCoord).x;
	color += (gaussVal - 0.5);
	
	FragColor = vec4(color, 1.0f);
} 