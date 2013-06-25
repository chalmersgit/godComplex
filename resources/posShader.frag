#version 120

uniform sampler2D posTex;
uniform sampler2D velTex;
uniform sampler2D infTex;

uniform sampler2D oPosTex;
uniform sampler2D noiseTex;

uniform sampler2D spriteTex;
varying vec4 texCoord;

varying float age;

uniform vec2 sample_offset;

uniform int maxControllers;
uniform vec2 controllers[16];
uniform float controllerMinIndices[16];
uniform float controllerMaxIndices[16];
uniform float controllerAlpha[16];

uniform float cloudCover;
uniform float cloudSharpness; 
uniform float testAlpha;
uniform int noiseLevel;
uniform int noiseMultiplier;
uniform int posDivide;

uniform float cloudColor;

float M_PI = 3.1415926535897932384626433832795;

float cloudExpCurve(float v){
	float c = v - cloudCover;
	if(c < 0.0){
		c = 0.0;
	}
	float cloudDensity = 1.0 - (pow(cloudSharpness, c) * 1.0);
	return cloudDensity;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec3 pos = texture2D( posTex, texCoord.st).rgb;
	float alph = 1.0;
	
	vec3 origPos = texture2D( oPosTex, texCoord.st ).rgb;
	
	float alphMultiplier = 1.0f;
	vec2 tempPos = vec2(0.0, 0.0);
    for(int i = 0; i < maxControllers; i++){
		float maxValue = controllerMaxIndices[i];
		if(maxValue >= 1.0){maxValue = 2.0;}
        if(origPos.x >= controllerMinIndices[i] && origPos.x < maxValue){
			alphMultiplier = controllerAlpha[i];
			break;
        }
	}
	
	vec2 noise = vec2( 0.0, 0.0);
	for(int i = 0; i <= noiseLevel; i++){
		noise += texture2D( noiseTex, pos.xy + i).rg;
	}
	alph = cloudExpCurve(testAlpha + noise.y * noiseMultiplier);

	gl_FragColor = vec4(cloudColor, cloudColor, cloudColor, alph*alphMultiplier);
}

