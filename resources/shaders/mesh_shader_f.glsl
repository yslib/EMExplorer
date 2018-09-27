#version 330
in vec3 outNormal;
in vec3 fragPos;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 objectColor;
out vec4 fragColor;
void main(){
    const vec3 lightColor = vec3(1.0,1.0,1.0);
    //ambient
    float ambient = 0.3;
    
    //diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
	vec3 normal;
	if(gl_FrontFacing)
       normal = normalize(outNormal);
	else
	   normal = normalize(-outNormal); 
    float diff =  max(dot(normal,lightDir),0);       //diffuse

    //specular
    vec3 reflDir = reflect(-lightDir,outNormal);
    vec3 viewDir = normalize(viewPos-fragPos);
    float spec = pow(max(dot(viewDir,reflDir),0),10);
    
    fragColor = (ambient+diff+spec)*objectColor;
}