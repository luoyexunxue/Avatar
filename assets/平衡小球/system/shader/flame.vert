uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform float uSpeed;
uniform float uTime;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec4 vColor;
out vec2 vTexCoord;
out vec2 vDistortionCoord0;
out vec2 vDistortionCoord1;
out vec2 vDistortionCoord2;
out float vFogDistance;

void main()
{
	vec4 position = uViewMatrix * uModelMatrix * aPosition;
	vec3 layerSpeed = vec3(0.2, 0.52, 0.1) * uSpeed;
	vTexCoord = vec2(aTexCoord.s, aTexCoord.t - 0.1);
	vDistortionCoord0 = vec2(aTexCoord.s, aTexCoord.t + layerSpeed.x * uTime);
	vDistortionCoord1 = vec2(aTexCoord.s, aTexCoord.t + layerSpeed.y * uTime);
	vDistortionCoord2 = vec2(aTexCoord.s, aTexCoord.t + layerSpeed.z * uTime);
	vColor = aColor;
#ifdef ENABLE_FOG
	vFogDistance = position.z;
#endif
	gl_Position = uProjMatrix * position;
}