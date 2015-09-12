uniform sampler2D tex;
uniform vec2 screen;
void main(void)
{
	vec2 uv = (gl_TexCoord[0].xy-.5);
	vec2 offset = normalize(uv)*pow(length(uv),4.)/50.;
	vec2 offset2 = uv/70.;
	vec4 c = vec4(0.);
	for(int i=0; i<16; i++)
	{
		//RGB distortion
		c.r += texture2D(tex,gl_TexCoord[0].xy+offset*float(i)+offset2).r;
		c.g += texture2D(tex,gl_TexCoord[0].xy+offset*float(i)).g;
		c.b += texture2D(tex,gl_TexCoord[0].xy+offset*float(i)-offset2).b;
	}
	c /= 16.;
	gl_FragColor = c;
	gl_FragColor.a = 1.;
}
