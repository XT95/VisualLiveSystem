uniform sampler2D tex;
uniform float bass;
uniform vec2 screen;
void main(void)
{
	vec4 color = texture2D(tex,gl_TexCoord[0].xy);
	color += pow( texture2D(tex, gl_TexCoord[0].xy*vec2(1.,0.)).grba + texture2D(tex, gl_TexCoord[0].xy*vec2(1.,0.)+vec2(0.,1.)).bgra ,vec4(4.) )*30.*(.5+bass);
	gl_FragColor = color;

}
