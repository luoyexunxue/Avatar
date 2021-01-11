uniform sampler2D uTexture[5];
uniform sampler2D uShadowMap;
uniform vec3 uHeightRange;
uniform vec4 uLightPos;
uniform vec4 uLightDir;
uniform vec4 uLightColor;
uniform vec3 uFogColor;
uniform vec2 uFogParam;

in vec4 vColor;
in vec4 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;
in vec4 vShadowCoord;
in float vFogDistance;
out vec4 fragColor;

float shadow(vec2 uv, float depth)
{
	const float exponential = 100.0;
	float result = 1.0;
	if (uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0)
	{
		float compare = texture(uShadowMap, uv).r;
		result = clamp(exp(compare - depth * exponential), 0.0, 1.0);
	}
	return result;
}

vec3 blend(float height)
{
	vec2 dx = dFdx(vTexCoord.xy);
	vec2 dy = dFdy(vTexCoord.xy);
	vec3 color = vec3(0.0);
	vec3 blendColor = texture(uTexture[3], vTexCoord.xy).rgb;
	float factor = texture(uTexture[4], vTexCoord.zw).r;
	if (height < uHeightRange.x)
	{
		vec3 c1 = textureGrad(uTexture[0], vTexCoord.xy, dx, dy).rgb;
		color = mix(c1, blendColor, factor);
	}
	else if (height < uHeightRange.y)
	{
		float t = (height - uHeightRange.x) / (uHeightRange.y - uHeightRange.x);
		vec3 c1 = textureGrad(uTexture[0], vTexCoord.xy, dx, dy).rgb;
		vec3 c2 = textureGrad(uTexture[1], vTexCoord.xy, dx, dy).rgb;
		color = mix(mix(c1, c2, t), blendColor, factor);
	}
	else if (height < uHeightRange.z)
	{
		float t = (height - uHeightRange.y) / (uHeightRange.z - uHeightRange.y);
		vec3 c2 = textureGrad(uTexture[1], vTexCoord.xy, dx, dy).rgb;
		vec3 c3 = textureGrad(uTexture[2], vTexCoord.xy, dx, dy).rgb;
		color = mix(mix(c2, c3, t), blendColor, factor);
	}
	else
	{
		vec3 c3 = textureGrad(uTexture[2], vTexCoord.xy, dx, dy).rgb;
		color = mix(c3, blendColor, factor);
	}
	return color;
}

void main()
{
	// 光照计算
	float attenuation = 1.0;
	vec3 lightDir = -uLightDir.xyz;
	if (uLightPos.w != 0.0)
	{
		vec3 dir = uLightPos.xyz - vPosition;
		attenuation = pow(max(1.0 - length(dir) / uLightColor.a, 0.1), 2.0);
		lightDir = normalize(dir);
	}
	if (uLightDir.w > 0.0)
	{
		float spotFactor = max(dot(lightDir, -uLightDir.xyz), 0.0);
		attenuation *= pow(max((spotFactor - uLightDir.w) / (1.0 - uLightDir.w), 0.1), 2.0);
	}
	attenuation *= max(dot(vNormal, lightDir) * 1.5, 0.1);

#ifdef ENABLE_SHADOW
	vec3 depth = vShadowCoord.xyz / vShadowCoord.w;
	attenuation *= shadow(depth.st, depth.z) * 0.5 + 0.5;
#endif

	vec4 color = vec4(blend(vPosition.z) * uLightColor.rgb * attenuation, 1.0) * vColor;

#ifdef ENABLE_FOG
	float fog = clamp((uFogParam.y + vFogDistance) / (uFogParam.y - uFogParam.x), 0.0, 1.0);
	color.rgb = fog * color.rgb + (1.0 - fog) * uFogColor;
#endif
	fragColor = color;
}