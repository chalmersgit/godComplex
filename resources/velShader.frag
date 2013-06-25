#extension GL_EXT_gpu_shader4: enable

uniform sampler2D positions;
uniform sampler2D velocities;
uniform sampler2D information;
uniform sampler2D oVelocities;
uniform sampler2D oPositions;
uniform sampler2D noiseTex;

uniform sampler2D oPositions2;

varying vec4 texCoord;

float tStep = 0.01;
float M_PI = 3.1415926535897932384626433832795;

uniform vec2 mousePos;

uniform vec2 leapFingersPos[40];
uniform vec2 leapFingersVel[40];
uniform int maxFingers;

uniform float fingerRadius;

uniform int maxControllers;
uniform vec2 prevControllers[16];
uniform vec2 controllers[16];
uniform float controllerMinIndices[16];
uniform float controllerMaxIndices[16];
uniform float controllerAlpha[16];

uniform int checkUserInput;

//Leap scaling
uniform float scaleX;
uniform float scaleY;

uniform float cloudSize;
uniform float velSpeed;
uniform float accTimer;

uniform bool firstTime;

uniform float cloudWidth;
uniform float cloudHeight;

vec3 constructSquare(vec3 ov, vec3 v, float shift){
	if(ov.y >= 0.25 && ov.y < 0.5){ 
		v.y = v.y - 0.5;
	}
	else if(ov.y >= 0.5 && ov.y < 0.75){
		v.y = v.y - 0.75;
		v.x = v.x - shift;
	}
	else if(ov.y >= 0.75){ 
		v.y = v.y - 0.75;
		v.x = v.x - shift;
	}
	return v;
}

vec3 addVariation(vec3 v, vec2 controller){
	float thresh = 0.65;
	vec2 dirVal = vec2(v.x - controller.x, v.y - controller.y);
	float distSqrd = length(dirVal) * length(dirVal);
	float percent = distSqrd/0.25;
    float threshDelta = 1.0 - thresh;
	float adjustedPercent = ( percent - thresh )/threshDelta;
    float F = ( 1.0 - ( cos( adjustedPercent * M_PI*2.0) * -0.5 + 0.5 ) ) * 0.04;
    dirVal = normalize(dirVal) * F;
    v.x += dirVal.x;
	v.y += dirVal.y;
	
	return v;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
	vec3 pos = texture2D( positions, texCoord.st).rgb;
	float mass = texture2D( positions, texCoord.st).a;
    
	vec3 vel = texture2D( velocities, texCoord.st).rgb;
	float decay = texture2D( velocities, texCoord.st).a;
	
	float age = texture2D( information, texCoord.st).r;
	float maxAge = texture2D( information, texCoord.st).g;
	vec2 acc = texture2D( information, texCoord.st).ba;
	vec3 origPos = texture2D(oPositions, texCoord.st).rgb;
	
	
	vec2 noise = vec2( 0.0, 0.0);
	noise += texture2D( noiseTex, pos.xy).rg;
	noise += texture2D( noiseTex, (pos.xy+1)).rg;
	noise += texture2D( noiseTex, (pos.xy+2)).rg;
	noise += texture2D( noiseTex, (pos.xy+3)).rg;

	for(int i = 0; i < maxControllers; i++){
		float maxValue = controllerMaxIndices[i];
		if(maxValue >= 1.0){maxValue = 2.0;}
		if(origPos.x >= controllerMinIndices[i] && origPos.x < maxValue){
			float offset = controllerMaxIndices[i] - controllerMinIndices[i];
			float theta = rand(vec2((origPos.x - offset), origPos.y))*M_PI*2.0;
			float amt = max(offset-abs(origPos.x - offset),origPos.y);
			amt *= (cloudSize);
			
			pos.x = cos(theta)*(-amt)*cloudWidth + controllers[i].x;
			pos.y = -sin(theta)*(-amt)*cloudHeight + controllers[i].y;
			
			if(controllerAlpha[i] == 0){
				vel = texture2D(oVelocities, texCoord.st).rgb;
			}
			break;
		}
	}
	
	pos.x += vel.x;
	pos.y += vel.y;
	
	if(decay > 0){
		vel += vec3(acc, 0.0)*decay;
		decay -= tStep;
	}

	if(checkUserInput == -1){
		float x = (mousePos.x - (scaleX*pos.x)) * (mousePos.x - (scaleX*pos.x));
		float y = (mousePos.y - (scaleY*pos.y)) * (mousePos.y - (scaleY*pos.y));
		if( x+y < fingerRadius){
			acc.x += 0.0001;
			decay = accTimer;
		}
	}
	
	for(int i = 0; i < maxFingers; i++){
		float x = (leapFingersPos[i].x - (scaleX*pos.x)) * (leapFingersPos[i].x - (scaleX*pos.x));
		float y = (leapFingersPos[i].y - (scaleY*pos.y)) * (leapFingersPos[i].y - (scaleY*pos.y));
		if( x+y < fingerRadius){
			float gradForce = 1 - ((x+y)/fingerRadius);
			acc += (leapFingersVel[i] * velSpeed) * gradForce;
			decay = accTimer;
		}
	}

	gl_FragData[0] = vec4(pos, mass);
	gl_FragData[1] = vec4(vel, decay);
	gl_FragData[2] = vec4(age, maxAge, acc);
}







