uniform samplerCube uTexture;

in vec3 vTexCoord;
in vec3 vUpDirection;
out vec4 fragColor;

void main()
{
	const float PI = 3.14159265;
	vec3 normal = normalize(vTexCoord);
	vec3 binormal = normalize(cross(normal, vUpDirection));
	vec3 tangent = normalize(cross(binormal, normal));

	vec3 irradiance = vec3(0.0);
	float sampleDelta = 0.025;
	int sampleCount = 0;
	for (float x = 0.0; x < 2.0 * PI; x += sampleDelta)
	{
		for (float y = 0.0; y < 0.5 * PI; y += sampleDelta)
		{
			vec3 sampleVec = vec3(sin(y) * cos(x), sin(y) * sin(x), cos(y));
			vec3 texCoord = sampleVec.x * tangent + sampleVec.y * binormal + sampleVec.z * normal;
			irradiance += texture(uTexture, texCoord).rgb * cos(y) * sin(y);
			sampleCount++;
		}
	}
	fragColor = vec4(irradiance * (PI / float(sampleCount)), 1.0);
}