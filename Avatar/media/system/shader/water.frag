uniform sampler2D uTexture[2];
uniform float uWaveHeight;
uniform float uElapsedTime;
uniform vec2 uWaterScale;
uniform vec3 uWaterColor;
uniform vec4 uLightColor;
uniform vec3 uFogColor;
uniform vec2 uFogParam;

in vec2 vTexCoordNormal;
in vec3 vTexCoordReflect;
in vec3 vPosition;
in vec3 vNormal;
in vec4 vLightPos;
in vec4 vLightDir;
in float vFogDistance;
out vec4 fragColor;

vec3 getNoise(vec2 uv, float time)
{
	vec4 n0 = texture(uTexture[0], uv * 750.0 + vec2(time * 0.145, time * 0.095));
	vec4 n1 = texture(uTexture[0], uv * 800.0 - vec2(time * -0.131, time * 0.092));
	vec4 noise = (n0 + n1) - 1.0;
	return noise.xyz;
}

void main()
{
	vec3 normalColor = getNoise(vTexCoordNormal * uWaterScale, uElapsedTime);
	vec2 waveMovement = uWaveHeight * normalColor.xy;
	vec2 projTexCoord = vTexCoordReflect.xy / vTexCoordReflect.z + waveMovement;
	vec3 reflectiveColor = texture(uTexture[1], projTexCoord).rgb;

	// 光照计算
	vec3 lightDir = -vLightDir.xyz;
	if (vLightPos.w != 0.0)
	{
		lightDir = normalize(vLightPos.xyz - vPosition);
	}
	vec3 viewDir = normalize(-vPosition);
	vec3 halfDir = normalize(lightDir + viewDir);
	float specular = pow(max(dot(vNormal, halfDir), 0.0), 32.0) * 0.5;
	float fresnel = clamp(1.0 - abs(dot(vNormal, viewDir)) + specular, 0.0, 1.0);
	vec4 color = vec4(uWaterColor * reflectiveColor + uLightColor.rgb * specular, fresnel);

#ifdef ENABLE_FOG
	float fog = clamp((uFogParam.y + vFogDistance) / (uFogParam.y - uFogParam.x), 0.0, 1.0);
	color.rgb = fog * color.rgb + (1.0 - fog) * uFogColor;
#endif
	fragColor = color;
}