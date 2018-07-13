#extension GL_ARB_texture_rectangle : enable

// struct LIGHT_SOURCE {
//     vec3 position_;        // light position in world space
//     vec3 ambientColor_;    // ambient color (r,g,b)
//     vec3 diffuseColor_;    // diffuse color (r,g,b)
//     vec3 specularColor_;   // specular color (r,g,b)
//     vec3 attenuation_;     // attenuation (constant, linear, quadratic)
// };

// uniform LIGHT_SOURCE lightSource;


uniform sampler1D texTransfunc;
uniform sampler2DRect texStartPos;
uniform sampler2DRect texEndPos;
uniform sampler3D texVolume;
//uniform sampler3D texGradient;
uniform float step;
uniform float ka;
uniform float kd;
uniform float shininess;
uniform float ks;
//uniform vec3 lightPos;
//uniform vec3 viewDir;
uniform vec3 lightdir;
uniform vec3 halfway;

vec3 PhongShading(vec3 samplePos, vec3 diffuseColor)
{
	vec3 shadedValue=vec3(0,0,0);

	////�����ݶ�
 //	vec3 N;
 //	N.x = (texture3D(texVolume, samplePos+vec3(step,0,0) ).w - texture3D(texVolume, samplePos+vec3(-step,0,0) ).w) - 1.0;
 //	N.y = (texture3D(texVolume, samplePos+vec3(0,step,0) ).w - texture3D(texVolume, samplePos+vec3(0,-step,0) ).w) - 1.0;
 //	N.z = (texture3D(texVolume, samplePos+vec3(0,0,step) ).w - texture3D(texVolume, samplePos+vec3(0,0,-step) ).w) - 1.0;

	//vec3 N = texture3D(texGradient, samplePos).xyz;
	vec3 N = texture3D(texVolume, samplePos).xyz;
 	N = N * 2.0 - 1.0;
  	N = -normalize(N);

	//vec3 L = lightPos;
	//vec3 V = viewDir;
	vec3 L = lightdir;
	vec3 H = halfway;

	//specularcolor
	//vec3 H = normalize(V+L);
	float NdotH = pow(max(dot(N, H), 0.0), shininess);
	float NdotL = max(dot(N, L), 0.0);
	
	vec3 ambient = ka * diffuseColor.rgb;
	vec3 specular = ks * NdotH * vec3(1.0,1.0,1.0);
	vec3 diffuse = kd * NdotL * diffuseColor.rgb;	

	shadedValue = specular + diffuse + ambient;
	return shadedValue;
}

void main()
{
	vec3 rayStart = texture2DRect(texStartPos, gl_TexCoord[0].st).xyz;
    vec3 rayEnd = texture2DRect(texEndPos, gl_TexCoord[0].st).xyz;
    vec3 start2end = rayEnd - rayStart;
//    vec4 bg = vec4(0.156863, 0.156863, 0.156863, 1.0);
	vec4 bg = vec4(1.0, 1.0, 1.0, 1.0);

    if (start2end.x == 0.0 && start2end.y == 0.0 && start2end.z == 0.0) {
		gl_FragColor = bg; // Background Colors
        return;
    }
    vec4 color = vec4(0, 0, 0, 0);
    vec3 direction = normalize(start2end);
    float distance = dot(direction, start2end);
    int steps = int(distance / step);
    for(int i = 0; i < steps; ++i) {

        vec3 samplePoint  = rayStart + direction * step * (float(i) + 0.5);
        vec4 scalar = texture3D(texVolume, samplePoint).xyzw;
        vec4 sampledColor = texture1D(texTransfunc, scalar.a);
		sampledColor.rgb = PhongShading(samplePoint, sampledColor.rgb);
		
        color = color + sampledColor * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
        if(color.a > 0.99)
            break;
    }
	if(color.a == 0.0) discard;
	color = color + vec4(bg.rgb, 0.0) * (1.0 - color.a);
    color.a = 1.0;
    gl_FragColor = color;
}
