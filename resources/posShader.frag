///*
#version 120

uniform sampler2D posTex;
uniform sampler2D velTex;
uniform sampler2D infTex;
uniform sampler2D oPosTex;

uniform sampler2D spriteTex;
varying vec4 texCoord;

varying float age;

uniform vec2	sample_offset;

//What needs to happen:
//Uniform controller, take distance from controller to determine alpha
uniform int maxControllers;
uniform vec2 controllers[16];
uniform float controllerMinIndices[16];
uniform float controllerMaxIndices[16];

void main()
{
    vec3 origPos = texture2D( oPosTex, gl_TexCoord[0].st ).rgb;
    
    float alph = 1.0;
    
    for(int i = 0; i < maxControllers; i++){
		float maxValue = controllerMaxIndices[i];
		if(maxValue >= 1.0){maxValue = 2.0;}
        if(origPos.x >= controllerMinIndices[i] && origPos.x < maxValue){
            vec2 fromCenter = gl_TexCoord[0].xy - controllers[i];
            alph = length(fromCenter) / 150.0;
        }
	}

    if(alph > 0.8){
        alph = 0.8;
    }
	/*
     vec4 colFac = vec4(1.0);//texture2D(spriteTex, gl_PointCoord);
     //colFac.rgb *= texture2D( posTex, gl_TexCoord[0].st ).rgb;
     
     //colFac.a *= .35;
     
     
     //colFac = vec4(1, 1, 1, 1);
     colFac.a *= age;
     
     
     
     gl_FragColor = colFac;
     */
	
	/*
     vec2 sample_offsetVec2f = vec2(1.0/1920.0, 0.0);
     
     vec3 sum = vec3( 0.0, 0.0, 0.0 );
     sum += texture2D( spriteTex, gl_TexCoord[0].st + -10.0 * sample_offset ).rgb * 0.009167927656011385;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  -9.0 * sample_offset ).rgb * 0.014053461291849008;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  -8.0 * sample_offset ).rgb * 0.020595286319257878;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  -7.0 * sample_offset ).rgb * 0.028855245532226279;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  -6.0 * sample_offset ).rgb * 0.038650411513543079;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  -5.0 * sample_offset ).rgb * 0.049494378859311142;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  -4.0 * sample_offset ).rgb * 0.060594058578763078;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  -3.0 * sample_offset ).rgb * 0.070921288047096992;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  -2.0 * sample_offset ).rgb * 0.079358891804948081;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  -1.0 * sample_offset ).rgb * 0.084895951965930902;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +   0.0 * sample_offset ).rgb * 0.086826196862124602;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  +1.0 * sample_offset ).rgb * 0.084895951965930902;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  +2.0 * sample_offset ).rgb * 0.079358891804948081;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  +3.0 * sample_offset ).rgb * 0.070921288047096992;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  +4.0 * sample_offset ).rgb * 0.060594058578763078;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  +5.0 * sample_offset ).rgb * 0.049494378859311142;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  +6.0 * sample_offset ).rgb * 0.038650411513543079;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  +7.0 * sample_offset ).rgb * 0.028855245532226279;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  +8.0 * sample_offset ).rgb * 0.020595286319257878;
     sum += texture2D( spriteTex, gl_TexCoord[0].st +  +9.0 * sample_offset ).rgb * 0.014053461291849008;
     sum += texture2D( spriteTex, gl_TexCoord[0].st + +10.0 * sample_offset ).rgb * 0.009167927656011385;
     
     gl_FragColor.rgb = sum;
     gl_FragColor.a = 1.0;*/
	
	
	//NOTE: Ollie testing
    //vec4 c = vec4(1.0);
    //float alph = 0.5 - smoothstep(0.1, 0.5, distance(gl_TexCoord[0].xy, vec2(0.5, 0.1)));
    gl_FragColor = vec4(1.0, 1.0, 1.0, alph);
}
//*/

