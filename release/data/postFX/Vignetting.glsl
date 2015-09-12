uniform sampler2D tex;
uniform vec2 screen;
void main(void)
{
	vec4 color = texture2D(tex,gl_TexCoord[0].xy);
	vec2 v = gl_TexCoord[0].xy*2.-1.;


     	color *= clamp(pow(clamp(1.-length(v*v*v)*.75,0.,1.)*1.5, 1.5)*2.5,0.,1.);
	gl_FragColor = color;

}
