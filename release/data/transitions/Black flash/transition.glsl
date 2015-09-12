varying vec2 v;
uniform sampler2D first,second;
uniform sampler1D spectrum1, spectrum2;
uniform float time;

void main()
{
    vec2 uv = v*.5+.5;
    vec3 col = texture2D(second,uv).rgb*time;
    gl_FragColor = vec4(col, 1.);
}
