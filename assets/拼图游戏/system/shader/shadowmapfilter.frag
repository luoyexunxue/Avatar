uniform sampler2D uTexture;
uniform vec2 uTextureSize;

in vec2 vTexCoord;
out vec4 fragColor;

void main()
{
	const float exponential = 100.0;
	float dx = 1.0 / uTextureSize.x;
	float dy = 1.0 / uTextureSize.y;
	float d0 = texture(uTexture, vTexCoord).r;
	float e1 = 0.1 * exp(exponential * (texture(uTexture, vTexCoord + vec2( dx,  dy)).r - d0));
	float e2 = 0.1 * exp(exponential * (texture(uTexture, vTexCoord + vec2( dx, -dy)).r - d0));
	float e3 = 0.1 * exp(exponential * (texture(uTexture, vTexCoord + vec2(-dx,  dy)).r - d0));
	float e4 = 0.1 * exp(exponential * (texture(uTexture, vTexCoord + vec2(-dx, -dy)).r - d0));
	float e5 = 0.1 * exp(exponential * (texture(uTexture, vTexCoord + vec2(0.0,  dy)).r - d0));
	float e6 = 0.1 * exp(exponential * (texture(uTexture, vTexCoord + vec2(0.0, -dy)).r - d0));
	float e7 = 0.1 * exp(exponential * (texture(uTexture, vTexCoord + vec2( dx, 0.0)).r - d0));
	float e8 = 0.1 * exp(exponential * (texture(uTexture, vTexCoord + vec2(-dx, 0.0)).r - d0));
	float result = exponential * d0 + log(0.2 + e1 + e2 + e3 + e4 + e5 + e6 + e7 + e8);
	fragColor = vec4(result);
}