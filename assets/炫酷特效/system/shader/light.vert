uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uShadowMatrix;
uniform vec3 uCameraPos;
uniform vec4 uLightPos;
uniform vec4 uLightDir;
uniform vec4 uBaseColor;

in vec4 aPosition;
in vec4 aNormal;
in vec2 aTexCoord;
in vec4 aColor;
out vec4 vColor;
out vec3 vPosition;
out vec3 vNormal;
out vec3 vWorldNormal;
out vec3 vWorldView;
out vec4 vLightPos;
out vec4 vLightDir;
out vec4 vShadowCoord;
out float vFogDistance;

void main()
{
	mat4 modelView = uViewMatrix * uModelMatrix;
	vec4 position = modelView * aPosition;
	vec4 normal = normalize(modelView * aNormal);
	vec4 wPosition = uModelMatrix * aPosition;
	vColor = aColor * uBaseColor;
	vPosition = position.xyz;
	vNormal = normal.xyz;
	vLightPos = uViewMatrix * uLightPos;
	vLightDir = uViewMatrix * vec4(uLightDir.xyz, 0.0);
	vLightDir.w = uLightDir.w;

#ifdef ENABLE_ENVIRONMENT
	vWorldNormal = normalize(uModelMatrix * aNormal).xyz;
	vWorldView = normalize(uCameraPos - wPosition.xyz);
#endif

#ifdef ENABLE_SHADOW
	vShadowCoord = uShadowMatrix * wPosition;
#endif

#ifdef ENABLE_FOG
	vFogDistance = position.z;
#endif
	gl_Position = uProjMatrix * position;
}