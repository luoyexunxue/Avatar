uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec3 vTexCoord;

void main()
{
	vec4 position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
	vTexCoord = aPosition.xyz;
	gl_Position = position.xyww;
}