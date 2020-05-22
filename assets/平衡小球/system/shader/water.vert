uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec4 uLightPos;
uniform vec4 uLightDir;

in vec4 aPosition;
in vec4 aNormal;
in vec2 aTexCoord;
in vec4 aColor;
out vec2 vTexCoordNormal;
out vec3 vTexCoordReflect;
out vec3 vPosition;
out vec3 vNormal;
out vec4 vLightPos;
out vec4 vLightDir;
out float vFogDistance;

void main()
{
	mat4 modelView = uViewMatrix * uModelMatrix;
	vec4 position = modelView * aPosition;
	vec4 normal = normalize(modelView * aNormal);
	vec4 finalPos = uProjMatrix * position;
	vTexCoordNormal = aTexCoord;
	vTexCoordReflect.x = 0.5 * (finalPos.w - finalPos.x);
	vTexCoordReflect.y = 0.5 * (finalPos.w + finalPos.y);
	vTexCoordReflect.z = finalPos.w;
	vPosition = position.xyz;
	vNormal = normal.xyz;
	vLightPos = uViewMatrix * uLightPos;
	vLightDir = uViewMatrix * vec4(uLightDir.xyz, 0.0);
	vLightDir.w = uLightDir.w;

#ifdef ENABLE_FOG
	vFogDistance = position.z;
#endif
	gl_Position = finalPos;
}