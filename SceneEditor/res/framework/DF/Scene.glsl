//-----------------------------------------------------------------------------
// Frameworks
//-----------------------------------------------------------------------------
varying vec2 v;
uniform float time;
uniform sampler2D rnd, tex;

//Some defines
#define ENABLE_REFLECTION
#define ENABLE_REFRACTION
#define ENABLE_VOLUME_RAYMARCHING
#define ENABLE_SKYDOME

//Must be implemented
#ifdef ENABLE_REFLECTION
	float mapReflected( in vec3 p );
#endif
#ifdef ENABLE_REFRACTION
	float mapRefracted( in vec3 p );
#endif
float mapSolid( in vec3 p );
float map( in vec3 p );
float mapVolume( in vec3 p );
vec3 shade( in vec4 p, in vec3 n, in vec3 org, in vec3 dir );
vec3 color( in vec4 p, in vec3 n, in vec3 org, in vec3 dir, in int ite );

//Maths utils
const float PI = 3.141592654;
mat3 rotate( in vec3 v, in float angle);
float noise( in vec2 p );
float noise( in vec3 p );
float fnoise( in vec3 p); //Fast smooth noise
float fbm( in vec3 p );

//Signed distance objects
float box( in vec3 p, in vec3 pos, in vec3 size );
float sphere( in vec3 p, in vec3 pos, in float s );
float torus( in vec3 p, in vec3 pos, in vec2 data );
float triforce( in vec3 p, in vec3 pos, in vec2 size);

//Classic raymarching utils
vec4 raymarcheOut( in vec3 org, in vec3 dir, in vec2 nearFar, in int nbStep, in float eps=0.01 );
vec4 raymarcheIn( in vec3 org, in vec3 dir, in vec2 nearFar, in int nbStep, in float eps=0.01 );
vec3 normal( in vec3 p, in float e = 0.001 );
float ambiantOcclusion( in vec3 p, in vec3 n, in float d=1., in int nbIte=6 );

//Volume raymarching utils
#ifdef ENABLE_VOLUME_RAYMARCHING
	vec4 raymarcheVolume(in vec3 org, in vec3 dir, in vec2 farCamLight, in int nbStep=32, in int nbStepLight=6);
	
	vec3 volumeLightColor = vec3(1.)*6.;
	vec3 volumeLightDir = normalize(vec3(1.,.5,1.5));
	vec3 volumeColor = vec3(0.);
	vec3 volumeAmbiantColor = vec3(1.);
	float volumeAbsorption = 100.;
#endif

//Skydome
#ifdef ENABLE_SKYDOME
	void skyInit();
	vec3 skyProcedural( in vec3 origin, in vec3 direction );
	
	float daytime=6.+mod(time*.1,12.);
	vec3 sunDirection = vec3(0.0);
	vec3 sunColor = vec3(0.0);
#endif



//-----------------------------------------------------------------------------
// Main functions
//-----------------------------------------------------------------------------
void main()
{
	//Init..
	#ifdef ENABLE_SKYDOME
		skyInit();
		#ifdef ENABLE_VOLUME_RAYMARCHING
			volumeLightColor = sunColor*5.;
			volumeLightDir = sunDirection;
			volumeColor = vec3(1.,.3,.1);
		#endif
	#endif

	//Camera
    vec3 org = gl_ModelViewMatrixInverse[3].xyz;
    vec3 dir = vec3( normalize( gl_ModelViewMatrixInverse*vec4(v.xy*vec2(16.0/9.0,1),-1.5,0.0) ) );

    //Compute pixel
    vec4 p = raymarcheOut(org,dir, vec2(.1,50.), 64);
    vec3 n = normal(p.xyz);
    vec3 col = color(p,n, org,dir, 5);

    //Gamma correction
    col = pow( col, vec3(1.75) );
    
    gl_FragColor = vec4(col, 1.);
}



//-----------------------------------------------------------------------------
// Maps
//-----------------------------------------------------------------------------
float mapSolid( in vec3 p )
{
	float d = p.y;
	//d = min(d, max(length(p.xz)-1.5,p.y-5.));
	//d = min(d, triforcefnoise(p,vec3(5.,1.,5.), vec2(1., .25)) );
	
	return d;
}

#ifdef ENABLE_REFLECTION
float mapReflected( in vec3 p )
{
	float d = triforce(p,vec3(-5.,1.,0.),vec2(1.,.25));
	d = min(d, sphere(p,vec3(0.,1.,-3.),.8) );
	
	
	return d;
}
#endif

#ifdef ENABLE_REFRACTION
float mapRefracted( in vec3 p )
{
	
	float d =  box(rotate(vec3(0.,1.,0.),60.)*(p-vec3(0.,1.,-3.)),vec3(0.),vec3(1.));
	d = max(d, -sphere(p,vec3(-0.,1.,-3.),1.2) );
	d = min(d, sphere(p,vec3(-5.,1.,0.),.5));
	//d = min(d, sphere(p,vec3(-3.,.5,-3.),.4) );
	
	return d;
}
#endif

float map( in vec3 p )
{
    float d = mapSolid(p);
    #ifdef ENABLE_REFLECTION
    	d = min( d, mapReflected(p) );
    #endif
    #ifdef ENABLE_REFRACTION
    d = min( d, mapRefracted(p) );
    #endif
    return d;
}
float mapVolume( in vec3 p )
{
	float d = sphere(p, vec3(0.,0.,0.), 1.)/clamp(abs(p.y)*2.,1.,10.)-fbm(rotate(vec3(0.,1.,0.),length(p.xz)*50.+p.y*50.)*p+vec3(0.,-time,0.));
	
	return d;
}






//-----------------------------------------------------------------------------
// Shading a point
//-----------------------------------------------------------------------------
vec3 shade( in vec4 p, in vec3 n, in vec3 org, in vec3 dir )
{	
    vec3 col = vec3(.5);
    
	if(mapSolid(p.xyz)<.1)
		col = texture2D( tex, p.xz*.5 ).rgb*.5;  
		
    col += sunColor*max( dot(n, sunDirection), 0.)*.5;
     
		
    col *= ambiantOcclusion(p.xyz,n,4.,6);
    //col *= ambiantOcclusion(p.xyz,sunDirection,4.,6);
    
    return col;
}

vec3 color( in vec4 p, in vec3 n, in vec3 org, in vec3 dir, in int nbIte )
{
	
	float t = length(org-p.xyz);
	vec3 col = shade(p,n,org,dir);
	vec3 skyCol = skyProcedural(org,dir);
	
	//Compute volume if possible
   	 vec4 vcol=vec4(0.,0.,0.,1.);
	#ifdef ENABLE_VOLUME_RAYMARCHING
		vcol = raymarcheVolume(org,dir,vec2(8.,1.), 64,6);
	#endif
	
	
    if( length(p.xyz-org) < 50. )
    for(int i=0; i<nbIte; i++)
    {
    	float solid = mapSolid(p.xyz);
    	float reflected = 99999.;
    	float refracted = 99999.;
    	#ifdef ENABLE_REFLECTION
    		reflected = mapReflected(p.xyz);
    	#endif
    	#ifdef ENABLE_REFRACTION
    		refracted = mapRefracted(p.xyz);
    	#endif
    	
    	
    	//Reflected objects
    	#ifdef ENABLE_REFLECTION
    	if( reflected < solid && reflected < refracted && reflected <.1 )
    	{
    		org = p.xyz;
    		dir = reflect(dir, n);
    		
    		p = raymarcheIn(p.xyz,dir,vec2(.1,50.),32);
    		n = normal(p.xyz);
    		
    		vec3 newCol = shade(p,n, org, dir);
   	 		
   	 		vec4 vcol2=vec4(0.,0.,0.,1.);
   	 		#ifdef ENABLE_VOLUME_RAYMARCHING
   	 			vcol2 = raymarcheVolume(org,dir,vec2(8.,1.), 64,6);
				newCol = newCol*vcol2.w + vec3(vcol2.xyz*10.);
			#endif
    		newCol = mix(newCol, skyProcedural(org,dir), vec3(1.)*min(length(p.xyz-org)/50.,1.)*vcol2.w); 		
    		    		
   	 		col = mix(col, newCol, .5);
   		}
   		#endif
   		
   		#ifdef ENABLE_REFRACTION
   			#ifdef ENABLE_REFLECTION
   				else
   			#endif
   		#endif
   		
   		//Refracted objects
   		#ifdef ENABLE_REFRACTION
    	if(  refracted < solid && refracted < reflected && refracted<.1 )
    	{
    		float indice = 2.492;	
    		dir = refract(dir, n, 1./indice);
    		
    		p = raymarcheIn(p.xyz-n*0.025,dir,vec2(.0,50.),128);
    		org = p.xyz;
    		dir = refract(dir, n, indice);
    		
    		p = raymarcheOut(p.xyz+normal(p.xyz)*0.025,dir,vec2(.0,50.),128);
    		n = normal(p.xyz);
    	
   	 		vec3 newCol = shade(p,n, org,dir);
   	 		
   	 		vec4 vcol2=vec4(0.,0.,0.,1.);
   	 		#ifdef ENABLE_VOLUME_RAYMARCHING
   	 			vcol2 = raymarcheVolume(org+dir*.1,dir,vec2(8.,1.), 64,6);
				newCol = newCol*vcol2.w + vec3(vcol2.xyz*10.); 
			#endif    		

    		newCol = mix(newCol, skyProcedural(org,dir), vec3(1.)*min(length(p.xyz-org)/50.,1.)*vcol2.w); 	
   	 		col = mix(col, newCol, .5);   	 		
   		}
   		#endif
    }
    
	//Apply fog horizon
	col = mix(col, skyCol*(vcol.w), vec3(1.)*min(t/50.,1.));    

	//Add volume if possible    
    #ifdef ENABLE_VOLUME_RAYMARCHING
    	col = col*vcol.w + vec3(vcol.xyz*10.);
    #endif

    return col;
	
}










//-----------------------------------------------------------------------------
// Framework
//-----------------------------------------------------------------------------
mat3 rotate( in vec3 v, in float angle)
{
	float c = cos(radians(angle));
	float s = sin(radians(angle));
	
	return mat3(c + (1.0 - c) * v.x * v.x, (1.0 - c) * v.x * v.y - s * v.z, (1.0 - c) * v.x * v.z + s * v.y,
		(1.0 - c) * v.x * v.y + s * v.z, c + (1.0 - c) * v.y * v.y, (1.0 - c) * v.y * v.z - s * v.x,
		(1.0 - c) * v.x * v.z - s * v.y, (1.0 - c) * v.y * v.z + s * v.x, c + (1.0 - c) * v.z * v.z
		);
}




float noise( in vec2 x )
{
    vec2 p = floor(x);
    vec2 f = fract(x);
	f = f*f*(3.0-2.0*f);
	
	vec2 uv = (p.xy) + f.xy;
	return texture2D( rnd, (uv+ 0.5)/256.0, -100.0 ).x;
}


float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    
    vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
    vec2 rg = texture2D( rnd, (uv+0.5)/256.0, -100.0 ).yx;
    return mix( rg.x, rg.y, f.z );
}

float fnoise( in vec3 p )
{
    vec3 i = floor(p);
    vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);
    vec3 f = cos((p-i)*acos(-1.))*(-.5)+.5;
    a = mix(sin(cos(a)*a),sin(cos(1.+a)*(1.+a)), f.x);
    a.xy = mix(a.xz, a.yw, f.y);
    return mix(a.x, a.y, f.z);
}



const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );
float fbm( in vec3 p )
{
    float f  = 0.5000*noise( p ); p = m*p*2.01;
    f += 0.2500*noise( p ); p = m*p*2.02;
    f += 0.1250*noise( p ); p = m*p*2.03;
    f += 0.0625*noise( p ); p = m*p*2.01;
    return f;
}




float box( in vec3 p, in vec3 pos, in vec3 data )
{
    return max(max(abs(p.x-pos.x)-data.x,abs(p.y-pos.y)-data.y),abs(p.z-pos.z)-data.z);
}

float sphere( in vec3 p, in vec3 pos, in float size)
{
	return length(p-pos)-size;
}
float torus( in vec3 p, in vec3 pos, in vec2 data )
{
	vec2 q = vec2(length(p.xz-pos.xz)-data.x,p.y-pos.y);
	return length(q)-data.y;
}
float triforce( in vec3 p, in vec3 pos, in vec2 size)
{
	p -= pos;
	vec3 q=abs(p);
	
	float d1=max(q.z-size.y,max(q.x*0.866025+p.y*0.5,-p.y)-size.x);
	float d2=max(q.z-size.y*2.0,max(q.x*0.866025-p.y*0.5,p.y)-size.x*0.525);
	
	return max(d1,-d2);
}








vec4 raymarcheIn(in vec3 org, in vec3 dir, in vec2 nearFar, in int nbStep, in float eps)
{
	float pass = 1./float(nbStep);
	float d = 0.0, accd = 0.0;
	vec4 p = vec4(org+dir*nearFar.x,0.);
    
	for(int i=0; i<nbStep; i++)
	{
		d = abs(map(p.xyz));
		accd += d;
		
		p.xyz += dir*d;
		p.w += pass;
		
		if( d<eps || accd > nearFar.y)
			break;
	}
    
	return p;
}
vec4 raymarcheOut(in vec3 org, in vec3 dir, in vec2 nearFar, in int nbStep, in float eps)
{
	float pass = 1./float(nbStep);
	float d = 0.0, accd = 0.0;
	vec4 p = vec4(org+dir*nearFar.x,0.);
    
	for(int i=0; i<nbStep; i++)
	{
		d = map(p.xyz);
		accd += d;
		
		p.xyz += dir*d;
		p.w += pass;
		
		if( d<eps || accd > nearFar.y)
			break;
	}
    
	return p;
}


vec3 normal( in vec3 p, in float e )
{
    vec3 eps = vec3(e,0.0,0.0);
    return normalize(vec3(
        map(p+eps.xyy)-map(p-eps.xyy),
        map(p+eps.yxy)-map(p-eps.yxy),
        map(p+eps.yyx)-map(p-eps.yyx)
    ));
}
float ambiantOcclusion( in vec3 p, in vec3 n, in float d, in int ite )
{
    float dlt = 0.0;
    float oc = 0.0;
    
    for(int i=1; i<=ite; i++)
    {
		dlt = d*float(i)/float(ite);
		oc += (dlt - map(p+n*dlt))/exp(dlt);
    }
    oc /= float(ite);
    
    return clamp(pow(1.-oc,d), 0.0, 1.0);
}




#ifdef ENABLE_VOLUME_RAYMARCHING
vec4 raymarcheVolume(in vec3 org, in vec3 dir, in vec2 farCamLight, in int nbStep=32, in int nbStepLight=6)
{
	float step         = farCamLight.x/float(nbStep);
	float stepl         = farCamLight.y/float(nbStepLight);
	vec3 p             = org;
	float T            = 1.;
        
	vec3 col=vec3(0.);

	float accDSolid=0.;
    for(int i=0; i<nbStep; i++)
    {
    	float dSolid = map(org+dir*accDSolid);
    	accDSolid+=dSolid;
    	if(dSolid<0.025 && length(p-org)>accDSolid)
    		break;
        float density = -mapVolume(p);
        if(density>0.)
        {
                float tmp = density / float(nbStep);
                T *= 1. -tmp * volumeAbsorption;
                if( T <= 0.01)
                        break;
                        
        
                float Tl = 1.0;
                for(int j=0; j<nbStepLight; j++)
                {
                	vec3 pp = p+volumeLightDir*float(j)*stepl;
                        float densityLight = -mapVolume( pp);
                        if(densityLight>0.)
            				    Tl *= 1. - densityLight * volumeAbsorption/float(nbStep);
     					   if (Tl <= 0.01)
           				 break;
                }
                col += volumeColor*tmp*T*density*density*density*density*1000.;
                col += (volumeAmbiantColor + volumeLightColor*Tl)*tmp*T;
                
                p += dir*step;
        }
        else
        {
           p += dir*step * (1.-density);
        }
	}   
	return vec4(col,T);
}
#endif






void skyInit()
{
	skyProcedural(vec3(0.),vec3(0.));
	sunColor = skyProcedural(vec3(0.),sunDirection);
}
	
vec3 skyProcedural( in vec3 origin, in vec3 direction )
{
	const float a_exposure = 0.1;
	const float T=2.2;
	const float J=240.;//Mon anniv 
	const float latitude=43.17; //A marseille !
	
	
	vec3 zenith_direction = vec3(0.0, 1.0, 0.0);
	direction.y=max(direction.y,.0001);
	vec4 color = vec4(1.0);
	float d = 0.4093*sin(2.0*PI*(J-81.0)/368.0);
	float l = latitude*PI/180.0;
	float thetas = PI/2.0 - asin(sin(l)*sin(d)-cos(l)*cos(d)*cos(PI*daytime/12.0));
	thetas = clamp(thetas, 0.0, 1.62);
	float phis = atan(-cos(d)*sin(PI*daytime/12.0),(cos(l)*sin(d)-sin(l)*cos(d)*cos(PI*daytime/12.0)));

	sunDirection = vec3(sin(thetas)*cos(phis), cos(thetas), sin(thetas)*sin(phis));
	
	vec3 Tvec3 = vec3(T, 1.0, 0.0);
	
	// Should be transposed..
	vec3 A = mat3(-0.0193, -0.0167,  0.1787,
        	      -0.2592, -0.2608, -1.4630,
        	       0.0,     0.0,     0.0) * Tvec3;
	vec3 B = mat3(-0.0665, -0.0950, -0.3554,
                   0.0008,  0.0092,  0.4275,
                   0.0,     0.0,     0.0) * Tvec3;
    vec3 C = mat3(-0.0004, -0.0079, -0.2270,
                   0.2125,  0.2102,  5.3251,
                   0.0,     0.0,     0.0) * Tvec3;
    vec3 D = mat3(-0.0641, -0.0441,  0.1206,
                  -0.8989, -1.6537, -2.5771,
                   0.0,     0.0,     0.0) * Tvec3;
    vec3 E = mat3(-0.0033, -0.0109, -0.0670,
                   0.0452,  0.0529,  0.3703,
                   0.0,     0.0,     0.0) * Tvec3;

	vec3 zenith_xyY;
	vec4 Tvec4 = vec4(T*T, T, 1.0, 0.0);
	vec4 thetasvec = vec4(pow(thetas, 3.), pow(thetas, 2.), thetas, 1.);
	zenith_xyY.z = (4.0453*T - 4.9710)*tan((4.0/9.0 - T/120.0)*(PI-2.0*thetas)) - 0.2155*T + 2.4192;
	zenith_xyY.x = dot(thetasvec, mat4( 0.00166, -0.00375,  0.00209, 0.0,
	                                   -0.02903,  0.06377, -0.03202, 0.00394,
	                                    0.11693, -0.21196,  0.06052, 0.25886,
	                                    0.0,      0.0,      0.0,     0.0) * Tvec4);
	zenith_xyY.y = dot(thetasvec, mat4( 0.00275, -0.00610,  0.00317, 0.0,
	                                   -0.04214,  0.08970, -0.04153, 0.00516,
	                                    0.15346, -0.26756,  0.06670, 0.26688,
	                                    0.0,      0.0,      0.0,     0.0) * Tvec4 );

	float costheta = dot( zenith_direction, direction );
	float cosgamma = dot( sunDirection, direction );
	float cos2gamma = cosgamma*cosgamma;
	float gamma = acos(cosgamma);
	
	vec3 num = ( 1.0 + A * exp( B/costheta ) ) * ( 1.0 + C * exp( D*gamma ) + E * cos2gamma );
	vec3 den = ( 1.0 + A * exp( B ) ) * ( 1.0 + C * exp( D * thetas ) + E * pow(cos(thetas),2.) );
	vec3 xyY = num/den * zenith_xyY;
	
	xyY.z = 1.0 - exp( -a_exposure * xyY.z);
	
	vec3 XYZ;
	XYZ.x = (xyY.x	/ xyY.y) * xyY.z;
	XYZ.y = xyY.z;
	XYZ.z = ((1.0 - xyY.x - xyY.y) / xyY.y ) * xyY.z;
	
	// mat3 constructor is column major, so the Rec 709 matrix must be transposed
	vec3 rgb = mat3( 3.240479, -0.969256,  0.055648,
	                -1.537150,  1.875992, -0.204043,
	                -0.498535,  0.041556,  1.057311) * XYZ;
	vec3 sun = vec3(1.0,0.5,0.1)*pow(max(dot(direction,sunDirection),0.0),512.0);

	return clamp( rgb, vec3(0.0), vec3(1.0))+sun;
}
