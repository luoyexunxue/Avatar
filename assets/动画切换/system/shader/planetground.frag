uniform sampler2D uTexture[2];
uniform float uNightScale;

in vec3 vColor0;
in vec3 vColor1;
in vec3 vNormal;
in vec2 vTexCoord;
out vec4 fragColor;

void main()
{
	// 包含白天和黑夜地面纹理
	vec3 dayColor = texture(uTexture[0], vTexCoord).xyz;
	vec3 nightColor = texture(uTexture[1], vTexCoord).xyz;

	vec3 day = dayColor * vColor0;
	vec3 night = uNightScale * nightColor * nightColor * nightColor * (1.0 - vColor0);

	fragColor = vec4(vColor1, 1.0) + vec4(day + night, 1.0);
}