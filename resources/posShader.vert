uniform sampler2D posTex;
uniform sampler2D infTex;

uniform float scaleX;
uniform float scaleY;
varying float age;



float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec4 newVertexPos;
	vec4 dv;
    
	dv = texture2D( posTex, gl_MultiTexCoord0.st );
	
	age = texture2D(infTex, gl_MultiTexCoord0.st).r;
	
	//scale vertex position to screen size
	//newVertexPos = vec4(mousePos+(scaleX * dv.x), mousePos+(scaleY * dv.y), scaleX * dv.z, 1);
	
	//newVertexPos = vec4((scaleX * dv.x), (scaleY * dv.y), (scaleX * dv.z), 1.0);
    
	//ollie:
	newVertexPos = vec4((scaleX * dv.x), (scaleY * dv.y), (scaleX * sin(dv.z)*5.0), 1.0);
	
	//adjust point size, increasing size kills performance
	gl_PointSize = 8.0 - (1.0 * age);
    
	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor  = gl_Color;
}

