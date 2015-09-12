varying vec2 v;
uniform sampler2D lastBuffer;
uniform float time,bassTime,CC[4];

void main()
{
	vec2 uv = v*vec2(.5,.5)+vec2(.5);
    vec4 col = texture2D(lastBuffer, uv);
    
    gl_FragColor = col;
}
