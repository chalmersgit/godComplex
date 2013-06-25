
void main(){
	
	vec3 normal, lightDir;
	vec4 diffuse, ambient, global ambient;
	float NdotL;
	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	lightDir = normalize(vec3(gl_LightSource[0].position));
	NdotL = max(dot(normal, lightDir), 0.0);
	
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	gl_FrontColor = NdotL * diffuse + globalAmbient + ambient;
	
	gl_Position = ftransform();
	
}

