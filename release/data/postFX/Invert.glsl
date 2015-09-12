uniform sampler2D tex;
uniform vec2 screen;
void main(void)
{
	gl_FragColor = vec4(1.)-texture2D(tex,gl_TexCoord[0].xy);
}
