#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect texStartPos;
uniform sampler2DRect texEndPos;
uniform sampler3D texVolume;
uniform float step;

void main()
{
	vec3 rayStart = texture2DRect(texStartPos, gl_TexCoord[0].st).xyz;
    vec3 rayEnd = texture2DRect(texEndPos, gl_TexCoord[0].st).xyz;
    vec3 start2end = rayEnd - rayStart;
	vec4 bg = vec4(1.0, 1.0, 1.0, 1.0);
	//vec4 bg = vec4(0.156863, 0.156863, 0.156863, 1.0);
    
    if (start2end.x == 0.0 && start2end.y == 0.0 && start2end.z == 0.0) {
        gl_FragColor = bg; // Background Color
        return;
    }
    vec4 color = vec4(0, 0, 0, 0);
    vec3 direction = normalize(start2end);
    float distance = dot(direction, start2end);
    int steps = int(distance / step);
	float scalar = 0.0;
    for(int i = 0; i < steps; ++i) {
        vec3 samplePoint  = rayStart + direction * step * (float(i) + 0.5);
		vec4 xyzv = texture3D(texVolume, samplePoint);
        if(xyzv.a > scalar)
			scalar = xyzv.a;
    }

    gl_FragColor = vec4(scalar, scalar, scalar, scalar);
}