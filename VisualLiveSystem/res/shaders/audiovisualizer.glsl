varying vec2 v;
uniform float bassCutoff;
//uniform float time,bass,bassTime,
uniform sampler1D spectrum;

void main()
{
    ///vec2 uv = abs(v);
    vec2 mv = v + vec2(1,1); //-> -1 1 to 0 2
    mv = mv*0.5;  // -> 0 2 to 0 1
    float coord = mv.x*0.19; // -> 22100 Hz to 4200Hz

    //Attention normalisé pour pour FFT entre 0 et 1.0 SANS symétrie
    if((1 + 1.5*coord)*texture1D(spectrum, coord).x > mv.y ) {
        if (coord < bassCutoff)
            gl_FragColor = vec4(0.7+0.3*mv.y,(1.-mv.y)*.75,0.3,1.);
        else
            gl_FragColor = vec4(mv.y,(1.-mv.y)*.75,(1.-mv.y)*.1,1.);
    }
    else
        gl_FragColor = vec4(48.,47.,47.,255.)/255.;

   /* if( uv.x >.95 )
    {

		if(bass > uv.y ) 
	    		gl_FragColor = vec4(uv.y,(1.-uv.y)*.75,(1.-uv.y)*.1,1.);
		else
	    		gl_FragColor = vec4(48.,47.,47.,255.)/255.;
    }*/
}


