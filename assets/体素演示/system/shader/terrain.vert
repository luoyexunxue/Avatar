uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uShadowMatrix;
uniform vec2 uTextureScale[2];

in vec4 aPosition;
in vec4 aNormal;
in vec2 aTexCoord;
in vec4 aColor;
out vec4 vColor;
out vec4 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;
out vec4 vShadowCoord;
out float vFogDistance;

void main()
{
	vec4 position = uModelMatrix * aPosition;
	vec4 normal = normalize(uModelMatrix * aNormal);
	vColor = aColor;
	vTexCoord.xy = uTextureScale[0] * aTexCoord;
	vTexCoord.zw = uTextureScale[1] * aTexCoord;
	vPosition = position.xyz;
	vNormal = normal.xyz;

#ifdef ENABLE_SHADOW
	vShadowCoord = uShadowMatrix * position;
#endif
#ifdef ENABLE_FOG
	vFogDistance = (uViewMatrix * position).z;
#endif
	gl_Position = uProjMatrix * uViewMatrix * position;
}