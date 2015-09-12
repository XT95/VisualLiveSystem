varying vec2 v;
uniform sampler2D first,second;
uniform sampler1D spectrum1, spectrum2;
uniform float time;

void main()
{
    vec2 uv = v*.5+.5;
    vec3 col = texture2D(first,uv).rgb;
    

    	
    float t = time;
    
    
    if(t>3.)
    	col =  texture2D(second,uv).rgb + clamp((5.-t)*.5,0.,1.);
    
    if(uv.x<uv.y*.5 && 1.-t<uv.y)
    	col = mix(col, vec3(0.), clamp(t, 0.,1.) );
    	
    t = time - 1.;
    if(1.-uv.x<uv.y*.5 && 1.-t<1.-uv.y)
    	col = mix(col, vec3(0.), clamp(t, 0.,1.) );
    	
    if(time>3.)
    	col = mix(col, texture2D(second,uv).rgb, (time-4.));
    	
    	
    gl_FragColor = vec4(col, 1.);
}
