#version 400 core
in vec2 xTexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D noiseTexture;

uniform int radius;
uniform int width;
uniform int height;
uniform float colorShift;
uniform bool hasNoise;


// http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// apply mean filter
	if (radius > 0) {
		float offsetX = 1.0f / width;
		float offsetY = 1.0f / height;

		int pixelCount = 0;
		
		for (int x = -radius; x <= radius; x++) {
			for (int y = -radius; y <= radius; y++) {
				vec2 coords = vec2(xTexCoord.x + x * offsetX, xTexCoord.y + y * offsetY);
				color += texture(screenTexture, coords).rgba;
				pixelCount++;
			}
		}
		color /= pixelCount;
	} else {
		color = texture(screenTexture, xTexCoord);
	}

	
	// add noise
	if (hasNoise) {
		float gaussVal = texture(noiseTexture, xTexCoord).r;
		color.rgb += (gaussVal - 0.5);
	}
	
	
	//float test = texture(screenTexture, xTexCoord).a;
	//FragColor = vec4(test, test, test, 1.0f);
	//if (test > 1.0f) {
	//	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//}
	
	// color shift
	if (colorShift != 0) {
		vec3 hsvCol = rgb2hsv(color.rgb);
		hsvCol.x = hsvCol.x + colorShift;
		color.rgb = hsv2rgb(hsvCol);
	}

	FragColor = color;
	//FragColor = vec4(color.a, color.a, color.a, 1.0f);
	
} 