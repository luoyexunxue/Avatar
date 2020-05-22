in vec4 vColor;
out vec4 fragColor;

void main()
{
	if (vColor.a < 0.01) discard;
	fragColor = vColor;
}