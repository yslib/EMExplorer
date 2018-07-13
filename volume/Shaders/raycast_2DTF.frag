#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect texStartPos;
uniform sampler2DRect texEndPos;
uniform sampler2DRect texTransfunc;
uniform sampler3D texVolume;
uniform sampler3D texV1;
uniform sampler3D texV2;
uniform float step;

void main()
{
	vec3 rayStart = texture2DRect(texStartPos, gl_TexCoord[0].st).xyz;
    vec3 rayEnd = texture2DRect(texEndPos, gl_TexCoord[0].st).xyz;
    vec3 start2end = rayEnd - rayStart;
    vec4 bg = vec4(1.0, 1.0, 1.0, 1.0);//vec4(0.156863, 0.156863, 0.156863, 1.0); // Background Color
    if (start2end.x == 0.0 && start2end.y == 0.0 && start2end.z == 0.0) {
        gl_FragColor = bg;
        return;
    }
    vec4 color = vec4(0, 0, 0, 0);
    vec3 direction = normalize(start2end);
    float distance = dot(direction, start2end);
    int steps = int(distance / step);
    for(int i = 0; i < 2048; ++i) {
	    if(i >= steps)
	        break;
        vec3 samplePoint  = rayStart + direction * step * (float(i) + 0.5);
        float scalar = texture3D(texVolume, samplePoint).r;
        vec4 sampledColor = texture1D(texTransfunc, scalar);
       // sampledColor.a = 0.001;
     
        color = color + sampledColor * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
        if(color.a > 0.99)
            break;
    }
    
    color = color + vec4(0.156863, 0.156863, 0.156863, 0.0) * (1.0 - color.a);
    color.a = 1.0;
    gl_FragColor = color;
	//gl_FragColor = texture2DRect(texStartPos, gl_TexCoord[0].st);
}