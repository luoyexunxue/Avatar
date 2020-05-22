uniform sampler2D uTexture;
uniform vec4 uColor;

in vec2 vTexCoord;
out vec4 fragColor;

void main()
{
    fragColor = texture(uTexture, vTexCoord) * uColor;
}