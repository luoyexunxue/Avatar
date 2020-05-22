uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform float uPointSize;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec4 vColor;

void main()
{
	vColor = aColor;
	gl_PointSize = uPointSize;
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
}