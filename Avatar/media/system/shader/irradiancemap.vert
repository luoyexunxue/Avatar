uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec3 vTexCoord;
out vec3 vUpDirection;

void main()
{
	vec4 position = uProjMatrix * uViewMatrix * aPosition;
	vTexCoord = aPosition.xyz;
	vUpDirection = vec3(uViewMatrix[0][1], uViewMatrix[1][1], uViewMatrix[2][1]);
	gl_Position = position.xyww;
}