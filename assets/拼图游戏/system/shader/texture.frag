uniform sampler2D uTexture;
uniform vec3 uFogColor;
uniform vec2 uFogParam;

in vec4 vColor;
in vec2 vTexCoord;
in float vFogDistance;
out vec4 fragColor;

void main()
{
	vec4 color = texture(uTexture, vTexCoord) * vColor;
	if (color.a < 0.01) discard;

#ifdef ENABLE_FOG
	float fog = clamp((uFogParam.y + vFogDistance) / (uFogParam.y - uFogParam.x), 0.0, 1.0);
	color.rgb = fog * color.rgb + (1.0 - fog) * uFogColor;
#endif
	fragColor = color;
}