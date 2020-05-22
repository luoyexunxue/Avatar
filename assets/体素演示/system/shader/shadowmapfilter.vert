uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;
	gl_Position = aPosition;
}