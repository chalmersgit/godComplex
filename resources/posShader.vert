uniform sampler2D posTex;
uniform sampler2D infTex;

uniform float scaleX;
uniform float scaleY;
varying float age;

varying vec4 texCoord;

uniform float pointSize;

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
	
	newVertexPos = vec4((scaleX * dv.x), (scaleY * dv.y), (scaleX * sin(dv.z)*5.0), 1.0);
	
	gl_PointSize = (pointSize - (1.0 * age));
    
	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor  = gl_Color;
}

