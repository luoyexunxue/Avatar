uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform float uTotalRow;
uniform float uTotalCol;
uniform float uFrameRow;
uniform float uFrameCol;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec4 vColor;
out vec2 vTexCoord;
out float vFogDistance;

void main()
{
	vec4 position = uViewMatrix * uModelMatrix * aPosition;
	vTexCoord.s = (uFrameCol + aTexCoord.s) / uTotalCol;
	vTexCoord.t = (uFrameRow + aTexCoord.t) / uTotalRow;
	vColor = aColor;
#ifdef ENABLE_FOG
	vFogDistance = position.z;
#endif
	gl_Position = uProjMatrix * position;
}