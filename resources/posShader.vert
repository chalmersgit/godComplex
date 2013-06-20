uniform sampler2D posTex;
uniform sampler2D infTex;

uniform float scaleX;
uniform float scaleY;
varying float age;

varying vec4 texCoord;

uniform float pointSize;

//What needs to happen:
//Uniform controller, take distance from controller to determine alpha

uniform int maxControllers;
uniform vec2 controllers[16];
uniform float controllerMinIndices[16];
uniform float controllerMaxIndices[16];
uniform sampler2D oPosTex;
uniform sampler2D velTex;


float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	texCoord = gl_MultiTexCoord0;
	vec4 newVertexPos;
	vec4 dv;
    
	dv = texture2D( posTex, gl_MultiTexCoord0.st );
	
	age = texture2D(infTex, gl_MultiTexCoord0.st).r;
	
	//scale vertex position to screen size
	//newVertexPos = vec4((scaleX * dv.x), (scaleY * dv.y), (scaleX * dv.z), 1.0);
    
	//ollie:
	newVertexPos = vec4((scaleX * dv.x), (scaleY * dv.y), (scaleX * sin(dv.z)*5.0), 1.0);
	
	
	/*
	vec3 pos = texture2D( posTex, texCoord.st).rgb;
	vec3 origPos = texture2D( oPosTex, gl_TexCoord[0].st ).rgb;
	vec3 vel = texture2D( velTex, texCoord.st).rgb;
	float decay = texture2D( velTex, texCoord.st).a;
	vec2 tempPos = vec2(0.0, 0.0);
	float sizeScale = 1.0;
    for(int i = 0; i < maxControllers; i++){
		float maxValue = controllerMaxIndices[i];
		if(maxValue >= 1.0){maxValue = 2.0;}
        if(origPos.x >= controllerMinIndices[i] && origPos.x < maxValue){
			//Distance from center
            vec2 fromCenter = pos.xy - controllers[i];
			//normalize(fromCenter);
			float distScale = length(fromCenter); //TODO check division
            sizeScale = (1/distScale); // + (noise);// * 10.0);
			
			break;
        }
	}
	*/
	
	//adjust point size, increasing size kills performance
	gl_PointSize = (pointSize - (1.0 * age));
    
	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor  = gl_Color;
}

