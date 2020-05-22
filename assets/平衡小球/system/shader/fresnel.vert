uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec4 vColor;
out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;
out float vFogDistance;

void main()
{
	mat4 modelView = uViewMatrix * uModelMatrix;
	vec4 position = modelView * aPosition;
	vec4 normal = normalize(modelView * aNormal);
	vColor = aColor;
	vTexCoord = aTexCoord;
	vPosition = position.xyz;
	vNormal = normal.xyz;

#ifdef ENABLE_FOG
	vFogDistance = position.z;
#endif
	gl_Position = uProjMatrix * position;
}