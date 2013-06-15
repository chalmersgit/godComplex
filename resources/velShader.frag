#extension GL_EXT_gpu_shader4: enable

uniform sampler2D positions;
uniform sampler2D velocities;
uniform sampler2D information;
uniform sampler2D oVelocities;
uniform sampler2D oPositions;
uniform sampler2D noiseTex;

varying vec4 texCoord;

float tStep = 0.01;
float M_PI = 3.1415926535897932384626433832795;

uniform vec2 mousePos;

uniform vec2 finger1;
uniform vec2 finger2;
uniform vec2 finger3;
uniform vec2 finger4;
uniform vec2 finger5;

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

void main(){
	vec3 pos = texture2D( positions, texCoord.st).rgb;
	
	float mass = texture2D( positions, texCoord.st).a;
    
	vec3 vel = texture2D( velocities, texCoord.st).rgb;
	float decay = texture2D( velocities, texCoord.st).a;
    
	float age = texture2D( information, texCoord.st).r;
	float maxAge = texture2D( information, texCoord.st).g;
    
	vec2 noise = vec2( 0.0, 0.0);
	noise += texture2D( noiseTex, pos.xy).rg;
	noise += texture2D( noiseTex, pos.xy+1).rg;
	noise += texture2D( noiseTex, pos.xy+2).rg;
	noise += texture2D( noiseTex, pos.xy+3).rg;
	
	
	vec3 origPos = texture2D(oPositions, texCoord.st).rgb;
	
	//if(noise.x < 0.0){
	//	noise.x *= -1;
	//}
	vel += vec3(noise.x,noise.y,0.0);
	
	
	//Life/death cycle
	age += tStep;
	
	if( age >= maxAge ){
		vec3 origVel = texture2D(oVelocities, texCoord.st).rgb;
		age = 0.0;
		
		if(pos.x > 1.0 || pos.x < 0.0 || pos.y > 1.0 || pos.y < 0.0 ){
			pos = origPos;
		}
		vel = origVel;
	}
    
    
    
	//Shape particles
	for(int i = 0; i < maxControllers; i++){
		float maxValue = controllerMaxIndices[i];
		if(maxValue >= 1.0){maxValue = 2.0;}
		if(origPos.x >= controllerMinIndices[i] && origPos.x < maxValue){
			//pos.x = controllerMaxIndices[i] - origPos.x + controllers[i].x;
			//pos.y = origPos.y + controllers[i].y;
			
			
			float theta = rand(origPos.xy)*M_PI*2.0;
			float offset = controllerMaxIndices[i] - controllerMinIndices[i];
			float amt = max(offset-abs(offset-origPos.x), offset-abs(offset-origPos.y));
			//float amtx = origPos.x;
			//float amty = origPos.y;
			
			amt *= 0.05; //NOTE: cloud size
			//amty *= 0.5;
			
			pos.x =  cos(theta)*(-amt)*2.0 + controllers[i].x;
			pos.y =  -sin(theta)*(-amt)*2.0 + controllers[i].y;
			
			
			
            vec2 controllerDir = controllers[i] - prevControllers[i];
			normalize(controllerDir);
            vec2 particleFromCenter = pos.xy - controllers[i];
			normalize(particleFromCenter);
            float forceScaler = dot(controllerDir, particleFromCenter);
            forceScaler = (forceScaler + 1.0) / 2.0;
            
            forceScaler = 1.0 - forceScaler;
            forceScaler *= 10.0;
            vel.xy += (-controllerDir) * forceScaler;
            
			
			
			//pos.x =  cos(theta) + controllers[i].x;
			//pos.y =  - sin(theta) + controllers[i].y;
			
			//pos = constructSquare(origPos, pos, controllerMaxIndices[i] - controllerMinIndices[i]);
			//pos = addVariation(pos, controllers[i]);
            
            //break! add this for efficiency 
		}
	}
    
     
	/*
     if(origPos.x < 0.5 && origPos.y < 0.5){
     //Squares the particles
     
     float theta = rand(origPos.xy)*M_PI*2.0;
     float amt = max(.25-abs(.25-origPos.x), .25-abs(.25-origPos.y));//(maxAmt*2.0) - distance(vec2(0.25, 0.25) , origPos.xy);
     amt *= 0.5;//cloud size
     pos.x =   cos(theta)*(-amt)*2.0 + controller1.x;
     pos.y =  - sin(theta)*(-amt) + controller1.y;
     
     
     //Add variation
     float thresh = 0.25;
     vec2 dirVal = vec2(pos.x - controller1.x, pos.y - controller1.y);
     float distSqrd = length(dirVal) * length(dirVal);
     float percent = distSqrd/0.25;
     float threshDelta = 1.0 - thresh;
     float adjustedPercent = ( percent - thresh )/threshDelta;
     }*/
    
	
	/* Hard Coded
     //Line up the particles with their controllers
     if(origPos.x < 0.25){
     //Squares the particles
     pos.x = 0.25 - origPos.x + controller1.x;
     pos.y = origPos.y + controller1.y;
     
     pos = constructSquare(origPos, pos);
     pos = addVariation(pos, controller1);
     }
     if(origPos.x >= 0.25 && origPos.x < 0.5){
     pos.x = 0.5 - origPos.x + controller2.x;
     pos.y = origPos.y + controller2.y;
     
     pos = constructSquare(origPos, pos);
     pos = addVariation(pos, controller2);
     }
     
     if(origPos.x >= 0.5 && origPos.x < 0.75){
     pos.x = 0.75 - origPos.x + controller3.x;
     pos.y = origPos.y + controller3.y;
     
     pos = constructSquare(origPos, pos);
     pos = addVariation(pos, controller3);
     }
     if(origPos.x >= 0.75){
     pos.x = 1 - origPos.x + controller4.x;
     pos.y = origPos.y + controller4.y;
     
     pos = constructSquare(origPos, pos);
     pos = addVariation(pos, controller4);
     }
     */
	
	//Particle interaction
	if(checkUserInput == -1){
		float x = (mousePos.x - (scaleX*pos.x)) * (mousePos.x - (scaleX*pos.x));
		float y = (mousePos.y - (scaleY*pos.y)) * (mousePos.y - (scaleY*pos.y));
		if( x+y < 50.0){
			vel.x = -vel.x;
		}
	}
	if(checkUserInput >= 1){
		float x = (finger1.x - (scaleX*pos.x)) * (finger1.x - (scaleX*pos.x));
		float y = (finger1.y - (scaleY*pos.y)) * (finger1.y - (scaleY*pos.y));
		if( x+y < 50.0){
			vel.x = -vel.x;
		}
	}
	if(checkUserInput >= 2){
		float x = (finger2.x - (scaleX*pos.x)) * (finger2.x - (scaleX*pos.x));
		float y = (finger2.y - (scaleY*pos.y)) * (finger2.y - (scaleY*pos.y));
		if( x+y < 50.0){
			vel.x = -vel.x;
		}
	}
	if(checkUserInput >= 3){
		float x = (finger3.x - (scaleX*pos.x)) * (finger3.x - (scaleX*pos.x));
		float y = (finger3.y - (scaleY*pos.y)) * (finger3.y - (scaleY*pos.y));
		if( x+y < 50.0){
			vel.x = -vel.x;
		}
	}
	if(checkUserInput >= 4){
		float x = (finger4.x - (scaleX*pos.x)) * (finger4.x - (scaleX*pos.x));
		float y = (finger4.y - (scaleY*pos.y)) * (finger4.y - (scaleY*pos.y));
		if( x+y < 50.0){
			vel.x = -vel.x;
		}
	}
	if(checkUserInput >= 5){
		float x = (finger5.x - (scaleX*pos.x)) * (finger5.x - (scaleX*pos.x));
		float y = (finger5.y - (scaleY*pos.y)) * (finger5.y - (scaleY*pos.y));
		if( x+y < 50.0){
			vel.x = -vel.x;
		}
	}
    
	//Add noise to the particles
	pos.x += (vel.x);
	pos.y += (vel.y);
	
	
	
    
		//position + mass
	gl_FragData[0] = vec4(pos, mass);
    
	//velocity + decay
	gl_FragData[1] = vec4(vel, decay);
    
	//age information
	gl_FragData[2] = vec4(age, maxAge, 0.0, 1.0);
}
