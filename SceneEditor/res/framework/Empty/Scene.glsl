varying vec2 v;
uniform float time;
uniform float CC[4];


void main()
{
	gl_FragColor = vec4( v*0.5+0.5, 0., 1.);
}