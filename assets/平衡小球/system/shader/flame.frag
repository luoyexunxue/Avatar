uniform sampler2D uTexture;
uniform sampler2D uDistortion;
uniform sampler2D uAlphaMask;
uniform vec3 uFogColor;
uniform vec2 uFogParam;

in vec4 vColor;
in vec2 vTexCoord;
in vec2 vDistortionCoord0;
in vec2 vDistortionCoord1;
in vec2 vDistortionCoord2;
in float vFogDistance;
out vec4 fragColor;

void main()
{
	// 火焰扰动
	vec4 noise0 = vec4(2.0) * texture(uDistortion, vDistortionCoord0) - vec4(1.0);
	vec4 noise1 = vec4(2.0) * texture(uDistortion, vDistortionCoord1) - vec4(1.0);
	vec4 noise2 = vec4(2.0) * texture(uDistortion, vDistortionCoord2) - vec4(1.0);

	const float distortionAmount0  = 0.092;
	const float distortionAmount1  = 0.092;
	const float distortionAmount2  = 0.092;

	vec4 noiseSum = noise0 * distortionAmount0 + noise1 * distortionAmount1 + noise2 * distortionAmount2;
	vec4 perturbedBaseCoord = vec4(vTexCoord, 0.0, 1.0) + noiseSum * (vTexCoord.t * 0.3 + 0.39);

	// 透明蒙版
	vec4 opacity = texture(uAlphaMask, perturbedBaseCoord.xy);
	if (opacity.r < 0.1) discard;

	vec4 color = texture(uTexture, perturbedBaseCoord.xy) * 2.0;
	color = color * opacity * vColor;
	color.a = opacity.r;

#ifdef ENABLE_FOG
	float fog = clamp((uFogParam.y + vFogDistance) / (uFogParam.y - uFogParam.x), 0.0, 1.0);
	color.rgb = fog * color.rgb + (1.0 - fog) * uFogColor;
#endif
	fragColor = color;
}