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

//Leap
uniform vec2 leapFingersPos[40];
uniform vec2 leapFingersVel[40];
uniform int maxFingers;

uniform vec2 finger1;
uniform vec2 finger2;
uniform vec2 finger3;
uniform vec2 finger4;
uniform vec2 finger5;

uniform vec2 fingerVel1;
uniform vec2 fingerVel2;
uniform vec2 fingerVel3;
uniform vec2 fingerVel4;
uniform vec2 fingerVel5;

uniform float fingerRadius;

uniform int maxControllers;
uniform vec2 prevControllers[16];
uniform vec2 controllers[16];
uniform float controllerMinIndices[16];
uniform float controllerMaxIndices[16];


uniform vec2 controller1;
uniform vec2 controller2;
uniform vec2 controller3;
uniform vec2 controller4;

uniform int checkUserInput;

//Leap scaling
uniform float scaleX;
uniform float scaleY;

uniform float cloudSize;
uniform float velSpeed;
uniform float accTimer;

uniform bool firstTime;

/*
To make clouds, we need to not use velocity applied to particles.
The noise created by velocity makes it look like fur or hair.
We should move positions + colour the positions. So we should get particles
shaped like a cloud + the alpha shows multiple clumped spots in a single cloud.

From this, we should get nice looking clouds that are static within themselves.
THEN, we can start applying *some* velocity (very minor effects) to get some rumbling.
*/

//float shift = 0.2;
vec3 constructSquare(vec3 ov, vec3 v, float shift){
	if(ov.y >= 0.25 && ov.y < 0.5){ //move to top right
		v.y = v.y - 0.5;
	}
	else if(ov.y >= 0.5 && ov.y < 0.75){ //move to top left
		v.y = v.y - 0.75;
		v.x = v.x - shift;
	}
	else if(ov.y >= 0.75){ //move to bottom left
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
//vec3 vel = vec3( 0.0, 0.0, 0.0);
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
	//noise /= 4.0;
	
	//vel += vec3(noise.x,noise.y,0.0);
	//vel.x += 0.001;
	
	
	//Life/death cycle
	
	/*age += tStep;
	if( age >= maxAge ){
		vec3 origVel = texture2D(oVelocities, texCoord.st).rgb;
		age = 0.0;
		if(pos.x > 1.0 || pos.x < 0.0 || pos.y > 1.0 || pos.y < 0.0 ){
			pos = origPos;
		}
		vel = origVel;
	}*/
	
	//vec3 tempPos = vec3(0, 0, 0);
	//Shape particles
	for(int i = 0; i < maxControllers; i++){
		float maxValue = controllerMaxIndices[i];
		if(maxValue >= 1.0){maxValue = 2.0;}
		if(origPos.x >= controllerMinIndices[i] && origPos.x < maxValue){
			float offset = controllerMaxIndices[i] - controllerMinIndices[i];
			float theta = rand(vec2((origPos.x - offset), origPos.y))*M_PI*2.0;
			float amt = max(offset-abs(origPos.x - offset),origPos.y);
			amt *= (cloudSize);
			
			pos.x = cos(theta)*(-amt)*2.0 + controllers[i].x;
			pos.y = -sin(theta)*(-amt)*2.0 + controllers[i].y;
			break;
		}
	}
	
	
	pos.x += vel.x;
	pos.y += vel.y;
	
	if(decay > 0){
		vel += vec3(acc, 0.0)*decay;
		decay -= tStep;
	}
	
    //acc = vec2(0, -9.81); /* Constant acceleration: gravity */
    //vel = vel + acc * dt;    /* New, timestep-corrected velocity */
    //pos = pos + vel * dt;      /* New, timestep-corrected position */
	
	//Particle interaction
	if(checkUserInput == -1){
		float x = (mousePos.x - (scaleX*pos.x)) * (mousePos.x - (scaleX*pos.x));
		float y = (mousePos.y - (scaleY*pos.y)) * (mousePos.y - (scaleY*pos.y));
		if( x+y < fingerRadius){
			acc.x += 0.01;
		}
	}
	
	//uniform vec2 leapFingersPos[40];
	//uniform vec2 leapFingersVel[40];
	
	for(int i = 0; i < maxFingers; i++){
		float x = (leapFingersPos[i].x - (scaleX*pos.x)) * (leapFingersPos[i].x - (scaleX*pos.x));
		float y = (leapFingersPos[i].y - (scaleY*pos.y)) * (leapFingersPos[i].y - (scaleY*pos.y));
		if( x+y < fingerRadius){
			acc += leapFingersVel[i] * velSpeed;
			decay = accTimer;
		}
	}
	
	/*
	if(checkUserInput >= 1){
		float x1 = (finger1.x - (scaleX*pos.x)) * (finger1.x - (scaleX*pos.x));
		float y1 = (finger1.y - (scaleY*pos.y)) * (finger1.y - (scaleY*pos.y));
		if( x1+y1 < fingerRadius){
			//pos += vec3(fingerVel1,0.0) * velSpeed;
			vel += vec3(fingerVel1,0.0) * velSpeed;
			decay = 1.0;
		}
	}
	if(checkUserInput >= 2){
		float x2 = (finger2.x - (scaleX*pos.x)) * (finger2.x - (scaleX*pos.x));
		float y2 = (finger2.y - (scaleY*pos.y)) * (finger2.y - (scaleY*pos.y));
		if( x2+y2 < fingerRadius){
			//pos += vec3(fingerVel2,0.0) * velSpeed;
			vel += vec3(fingerVel2,0.0) * velSpeed;
			decay = 1.0;
		}
	}
	if(checkUserInput >= 3){
		float x3 = (finger3.x - (scaleX*pos.x)) * (finger3.x - (scaleX*pos.x));
		float y3 = (finger3.y - (scaleY*pos.y)) * (finger3.y - (scaleY*pos.y));
		if( x3+y3 < fingerRadius){
			//pos += vec3(fingerVel3,0.0) * velSpeed;
			vel += vec3(fingerVel3,0.0) * velSpeed;
			decay = 1.0;
		}
	}
	if(checkUserInput >= 4){
		float x4 = (finger4.x - (scaleX*pos.x)) * (finger4.x - (scaleX*pos.x));
		float y4 = (finger4.y - (scaleY*pos.y)) * (finger4.y - (scaleY*pos.y));
		if( x4+y4 < fingerRadius){
			//pos += vec3(fingerVel4,0.0) * velSpeed;
			vel += vec3(fingerVel4,0.0) * velSpeed;
			decay = 1.0;
		}
	}
	if(checkUserInput >= 5){
		float x5 = (finger5.x - (scaleX*pos.x)) * (finger5.x - (scaleX*pos.x));
		float y5 = (finger5.y - (scaleY*pos.y)) * (finger5.y - (scaleY*pos.y));
		if( x5+y5 < fingerRadius){
			//pos += vec3(fingerVel5,0.0) * velSpeed;
			vel += vec3(fingerVel5,0.0) * velSpeed;
			decay = 1.0;
		}
	}
    */
	
	//Add noise to the particles
	//pos.x += (vel.x);
	//pos.y += (vel.y);
    
	//position + mass
	gl_FragData[0] = vec4(pos, mass);
    
	//velocity + decay
	gl_FragData[1] = vec4(vel, decay);
    
	//age information
	gl_FragData[2] = vec4(age, maxAge, acc);
}
