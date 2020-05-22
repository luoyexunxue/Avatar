uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewProjMatrix;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec4 vPosition;
out vec2 vTexCoord;

void main()
{
	vec4 position = uViewProjMatrix * uModelMatrix * aPosition;
	vPosition = position;
	vTexCoord = aTexCoord;
	gl_Position = position;
}