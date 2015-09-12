varying vec2 v;
uniform sampler2D first,second;
uniform sampler1D spectrum1, spectrum2;
uniform float time;

void main()
{
    vec2 uv = v*.5+.5;
    vec3 col = texture2D(first,uv).rgb + vec3(1.)*mix(0.,1.,time*2.);
    if(time>.6)
        col = texture2D(second,uv).rgb + vec3(1.)*mix(1.,0.,(time-.6)*2.5);
    gl_FragColor = vec4(col, 1.);
}
