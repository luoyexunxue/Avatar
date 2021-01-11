uniform sampler2D uShadowMap;
uniform samplerCube uIrradianceMap;
uniform samplerCube uEnvironmentMap;
uniform sampler2D uBRDFIntegrationMap;
uniform vec4 uLightColor;
uniform vec3 uFogColor;
uniform vec2 uFogParam;
uniform float uMetalness;
uniform float uRoughness;

in vec4 vColor;
in vec3 vPosition;
in vec3 vNormal;
in vec3 vWorldNormal;
in vec3 vWorldView;
in vec4 vLightPos;
in vec4 vLightDir;
in vec4 vShadowCoord;
in float vFogDistance;
out vec4 fragColor;

vec3 brdf(vec3 diffuse, vec3 specular, float roughness, float dotNH, float dotLH)
{
	float alpha = roughness * roughness;
	float denom = dotNH * dotNH * (alpha - 1.0) + 1.0;
	float ndf = 0.31830988618 * alpha / (denom * denom);
	float fresnel = exp2(-(5.55473 * dotLH + 6.98316) * dotLH);
	vec3 schlick = specular + (1.0 - specular) * fresnel;
	vec3 diff = 0.31830988618 * diffuse;
	vec3 spec = schlick * ndf;
	return max(diff + spec, 0.0);
}

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

void main()
{
	if (vColor.a < 0.1) discard;

	// 光照计算
	float attenuation = 1.0;
	vec3 lightDir = -vLightDir.xyz;
	if (vLightPos.w != 0.0)
	{
		vec3 dir = vLightPos.xyz - vPosition;
		attenuation = pow(max(1.0 - length(dir) / uLightColor.a, 0.1), 2.0);
		lightDir = normalize(dir);
	}
	if (vLightDir.w > 0.0)
	{
		float spotFactor = max(dot(lightDir, -vLightDir.xyz), 0.0);
		attenuation *= pow(max((spotFactor - vLightDir.w) / (1.0 - vLightDir.w), 0.1), 2.0);
	}
#ifdef ENABLE_SHADOW
	vec3 depth = vShadowCoord.xyz / vShadowCoord.w;
	attenuation *= shadow(depth.st, depth.z) * 0.5 + 0.5;
#endif

	// 基于物理的 BRDF 计算
	vec3 halfDir = normalize(lightDir + normalize(-vPosition));
	float dotNL = max(dot(vNormal, lightDir), 0.08);
	float dotNH = max(dot(vNormal, halfDir), 0.0);
	float dotLH = max(dot(lightDir, halfDir), 0.0);
	float roughness = clamp(uRoughness, 0.04, 1.0);

	vec3 diffuse = vColor.rgb * (1.0 - uMetalness);
	vec3 specular = mix(vec3(0.04), vColor.rgb, uMetalness);
	vec3 radiance = uLightColor.rgb * (attenuation * dotNL * 3.14159265359);
	vec3 reflection = brdf(diffuse, specular, roughness, dotNH, dotLH) * radiance;

#ifdef ENABLE_ENVIRONMENT
	const float mipmapCount = 5.0;
	float dotNV = max(dot(vWorldNormal, vWorldView), 0.0);

	// 原公式 pow(1.0 - cosTheta, 5.0) 的快速版本
	float fresnel = exp2(-(5.55473 * dotNV + 6.98316) * dotNV);
	vec3 reflectDir = reflect(-vWorldView, vWorldNormal);
	vec3 irradiance = texture(uIrradianceMap, vWorldNormal).rgb;
	vec3 prefilteredColor = textureLod(uEnvironmentMap, reflectDir, roughness * mipmapCount).rgb;
	vec2 envBRDF  = texture(uBRDFIntegrationMap, vec2(dotNV, roughness)).xy;
	vec3 schlick = specular + (max(vec3(1.0 - roughness), specular) - specular) * fresnel;
	vec3 diff = diffuse * irradiance * (1.0 - schlick);
	vec3 spec = prefilteredColor * (schlick * envBRDF.x + envBRDF.y);
	reflection += diff + spec;
#endif
	vec4 color = vec4(reflection, vColor.a);

#ifdef ENABLE_FOG
	float fog = clamp((uFogParam.y + vFogDistance) / (uFogParam.y - uFogParam.x), 0.0, 1.0);
	color.rgb = fog * color.rgb + (1.0 - fog) * uFogColor;
#endif
	fragColor = color;
}