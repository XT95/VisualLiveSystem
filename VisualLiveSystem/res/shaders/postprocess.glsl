varying vec2 v;
uniform sampler2D texture;
uniform float white,black,motionBlur,
gamma,contrast,desaturate,brightness, fisheye;

void main()
{
        vec2 tmp = gl_TexCoord[0].xy*2.-1.;
        vec2 uv = tmp;
        uv.x /=  sqrt(sin((tmp.y * 0.5 + 0.5) * 3.14) * fisheye + 1.0);
        uv.y /=  sqrt(sin((tmp.x * 0.5 + 0.5) * 3.14) * fisheye + 1.0);
        uv = uv * 0.5 + 0.5;


        vec3 col = texture2D(texture,uv);
	col = pow( col, vec3(gamma) );
	vec3 gray = vec3(dot(col,vec3(0.33)));
	col *= 1.+brightness;
	col = mix(gray, col, 1.+contrast);
	col = mix( col, gray, desaturate );
	
	gl_FragColor.rgb = col*(1.-black)+white;
	gl_FragColor.a = 1.-motionBlur;
}
