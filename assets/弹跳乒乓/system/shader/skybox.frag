uniform samplerCube uTexture;
uniform vec3 uFogColor;
uniform vec2 uFogParam;

in vec3 vTexCoord;
out vec4 fragColor;

void main()
{
#ifdef ENABLE_FOG
	fragColor = vec4(uFogColor, 1.0);
#else
	fragColor = texture(uTexture, vTexCoord);
#endif
}