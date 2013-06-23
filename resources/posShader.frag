
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

//What needs to happen:
//Uniform controller, take distance from controller to determine alpha
uniform int maxControllers;
uniform vec2 controllers[16];
uniform float controllerMinIndices[16];
uniform float controllerMaxIndices[16];
uniform float controllerAlpha[16];

uniform float cloudCover; // = 0.5;
uniform float cloudSharpness; // = 0.5;
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
	/*
	vec3 vel = texture2D( velTex, texCoord.st).rgb;
	float decay = texture2D( velTex, texCoord.st).a;
	vec2 tempPos = vec2(0.0, 0.0);
    for(int i = 0; i < maxControllers; i++){
		float maxValue = controllerMaxIndices[i];
		if(maxValue >= 1.0){maxValue = 2.0;}
        if(origPos.x >= controllerMinIndices[i] && origPos.x < maxValue){
			float offset = 0;//controllerMaxIndices[i] - controllerMinIndices[i];
			float theta = rand(vec2(origPos.x, origPos.y))*M_PI*2.0;
			float amt = max(origPos.x,origPos.y);
			amt *= (0.08);
			tempPos.x =  cos(theta)*(-amt)*2.0;// + controllers[i].x;
			tempPos.y =  -sin(theta)*(-amt)*2.0;// + controllers[i].y;
			
			//Distance from center
            vec2 fromCenter = pos.xy - controllers[i];
			//normalize(fromCenter);
			float distScale = length(fromCenter); //TODO check division
            alph = (0.0001/distScale); // + (noise);// * 10.0);
			
			break;
        }
	}
	*/
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

	
	//NOTE: Ollie testing
    //vec4 c = vec4(1.0);
    //float pointFalloff = 0.5 - smoothstep(0.1, 0.5, distance(gl_TexCoord[0].xy, vec2(0.5, 0.1)));
    gl_FragColor = vec4(cloudColor, cloudColor, cloudColor, alph*alphMultiplier);
}

