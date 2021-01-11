uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uCameraPos;
uniform vec4 uLightPos;
uniform vec3 uInvWavelength;
uniform float uCameraHeight2;
uniform float uOuterRadius;
uniform float uOuterRadius2;
uniform float uInnerRadius;
uniform float uKrESun;
uniform float uKmESun;
uniform float uKr4PI;
uniform float uKm4PI;
uniform float uScale;
uniform float uScaleDepth;
uniform float uScaleOverScaleDepth;

in vec4 aPosition;
in vec4 aNormal;
in vec2 aTexCoord;
in vec4 aColor;
out vec3 vDirection;
out vec3 vColor0;
out vec3 vColor1;
out vec3 vNormal;
out vec2 vTexCoord;

const int nSamples = 3;
const float fSamples = 3.0;

float scale(float fCos)
{
	float x = 1.0 - fCos;
	return uScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main()
{
	vec3 position = vec3(uModelMatrix * aPosition);
	vec3 cameraPos = uCameraPos - vec3(uModelMatrix[3]);
	
	// Get the ray from the camera to the vertex and its length
	// (which is the far point of the ray passing through the atmosphere)
	vec3 v3Ray = position - uCameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the closest intersection of the ray with the outer atmosphere
	// (which is the near point of the ray passing through the atmosphere)
	float fMiddle = -dot(cameraPos, v3Ray);
	float fDet = max(0.0, uOuterRadius2 + fMiddle * fMiddle - uCameraHeight2);
	float fNear = fMiddle - sqrt(fDet);

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = cameraPos + v3Ray * fNear;
	fFar -= fNear;
	vec3 posToCenter = position - vec3(uModelMatrix[3]);
	float fDepth = exp((uInnerRadius - uOuterRadius) / uScaleDepth);
	float fCameraAngle = dot(-v3Ray, posToCenter) / uInnerRadius;
	float fLightAngle = dot(-uLightPos.xyz, posToCenter) / uInnerRadius;
	float fCameraScale = scale(fCameraAngle);
	float fLightScale = scale(fLightAngle);
	float fCameraOffset = fDepth * fCameraScale;
	float fTemp = (fLightScale + fCameraScale);

	// Initialize the scattering loop variables
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * uScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	vec3 v3Attenuate;
	for(int i = 0; i < nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(uScaleOverScaleDepth * (uInnerRadius - fHeight));
		float fScatter = fDepth * fTemp - fCameraOffset;
		v3Attenuate = exp(-fScatter * (uInvWavelength * uKr4PI + uKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	gl_Position = uProjMatrix * uViewMatrix * vec4(position, 1.0);
	// Calculate the attenuation factor for the ground
	vColor0 = v3Attenuate;
	vColor1 = v3FrontColor * (uInvWavelength * uKrESun + uKmESun);
	vTexCoord = aTexCoord;
	vNormal = vec3(uModelMatrix * aNormal);
}