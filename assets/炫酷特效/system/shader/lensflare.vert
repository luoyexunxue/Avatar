uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec2 uCenterPos;
uniform vec2 uScreenPos;
uniform float uScale;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec2 vTexCoord;

void main()
{
	float aspect = uCenterPos[0] / uCenterPos[1];
	float x = uScreenPos[0] / uCenterPos[0] + aPosition[0] * uScale;
	float y = uScreenPos[1] / uCenterPos[1] + aPosition[1] * uScale * aspect;
	vTexCoord = aTexCoord;
	gl_Position = vec4(x, y, 0, 1.0);
}