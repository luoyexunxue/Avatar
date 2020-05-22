uniform sampler2D uAlphaMap;

in vec4 vPosition;
in vec2 vTexCoord;
out vec4 fragColor;

void main()
{
	if (texture(uAlphaMap, vTexCoord).a < 0.5) discard;

	const float bias = 0.0001;
	float zcoord = vPosition.z / vPosition.w;
	float depth = zcoord * 0.5 + 0.5 + bias;
	fragColor = vec4(depth);
}