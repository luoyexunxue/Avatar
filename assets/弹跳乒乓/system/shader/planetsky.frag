uniform float uG;
uniform float uG2;

in vec3 vDirection;
in vec3 vColor0;
in vec3 vColor1;
in vec3 vLightPos;
out vec4 fragColor;

void main()
{
	float fCos = dot(vLightPos, vDirection) / length(vDirection);
	float fCos2 = fCos * fCos;
	float fMiePhase = 1.5 * ((1.0 - uG2) / (2.0 + uG2)) * (1.0 + fCos2) / pow(1.0 + uG2 - 2.0 * uG * fCos, 1.5);
	float fRayleighPhase = 0.75 + 0.75 * fCos2;
	vec3 color = fRayleighPhase * vColor0 + fMiePhase * vColor1;

 	fragColor = vec4(color, color.b);
}