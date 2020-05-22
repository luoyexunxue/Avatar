uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec2 uOffset;

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;
in vec4 aColor;
out vec4 vColor;
out vec2 vTexCoord;

void main()
{
	vec4 position = uModelMatrix * aPosition;
	position.x = position.x + uOffset.x;
	position.y = position.y + uOffset.y;
	position.z = 0.0;
	vTexCoord = aTexCoord;
	vColor = aColor;
	gl_Position = uProjMatrix * position;
}