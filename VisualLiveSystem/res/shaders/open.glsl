varying vec2 v;
uniform float time;
uniform float CC[4];
uniform sampler1D spectrum;
uniform float bassTime,bass;

//-----------------------------------------------------------------------------
// Maths Utils
//-----------------------------------------------------------------------------
#define PI 3.141592654

vec3 rotateY(vec3 v, float x)
{
    return vec3(
        cos(x)*v.x - sin(x)*v.z,
        v.y,
        sin(x)*v.x + cos(x)*v.z
    );
}

vec3 rotateX(vec3 v, float x)
{
    return vec3(
        v.x,
        v.y*cos(x) - v.z*sin(x),
        v.y*sin(x) + v.z*cos(x)
    );
}

vec3 rotateZ(vec3 v, float x)
{
    return vec3(
        v.x*cos(x) - v.y*sin(x),
        v.x*sin(x) + v.y*cos(x),
        v.z
    );
}


//-----------------------------------------------------------------------------
// Scene/Objects
//-----------------------------------------------------------------------------
float box(vec3 p, vec3 size)
{
        return max(max(abs(p.x)-size.x,abs(p.y)-size.y),abs(p.z)-size.z);
}
float center(vec3 p)
{
        return box(rotateX(rotateZ(rotateY(p-vec3(.0,2.7,.0),bassTime*.02+time*.3),bassTime*.02+time*.3),bassTime*.02+time*.3),vec3(.75+bass));
}
float scene(vec3 p)
{
        float d = p.y;
        d = min(d, p.x+5.);
        d = min(d, box(p,vec3(5.,1.,5.)) );
        d = max(d, -length(p.xz)+4.);
        d = min(d, max( min(p.y-sqrt(length(p.xz))*.5+texture1D(spectrum,0.).x+cos(length(p.xz)*10.)*.025,length((p+vec3(.0,texture1D(spectrum,0.).x-.25,.0))*vec3(1.,2.,1.))-1.5), length(p.xz)-4.) );
        d = min(d, center(p) );
        d = min(d, box(p-vec3(.0,2.,-5.0),vec3(50.,5.,.25)) );
        d = min(d, box(p-vec3(4.75,1.1,0.),vec3(.2,.2,CC[0]*5.)) );
        d = min(d, box(p-vec3(-4.75,1.1,0.),vec3(.2,.2,CC[1]*5.)) );
        d = min(d, box(p-vec3(.0,1.1,4.75),vec3(CC[2]*5.,.2,.2)) );
        d = min(d, box(p-vec3(.0,1.1,-4.75),vec3(CC[3]*5.,.2,.2)) );
        return d;
}


//-----------------------------------------------------------------------------
// Raymarching tools
//-----------------------------------------------------------------------------
vec3 raymarche(out float pass, vec3 org, vec3 dir, int step, float eps)
{
        vec3 p=org;
        float d;
        pass=0.0;
        for(int i=0; i<step; i++)
        {
                d = scene(p);
                p += d * dir;
                if( d < eps )
                        break;
        }
        return p;
}
vec3 getNormal(vec3 p)
{
        vec3 eps = vec3(0.001,0.0,0.0);
        return normalize(vec3(
        scene(p+eps.xyy)-scene(p-eps.xyy),
        scene(p+eps.yxy)-scene(p-eps.yxy),
        scene(p+eps.yyx)-scene(p-eps.yyx)
        ));
}
float getAO(vec3 p, vec3 n, vec2 a)
{
        float dlt = a.x;
        float oc = 0.0, d = a.y;
        for(int i = 0; i<6; i++)
        {
                oc += (float(i) * dlt - scene(p + n * float(i) * dlt)) / d;
                d *= 2.0;
        }
        return clamp(1.0 - oc, 0.0, 1.0);
}

float getAmbiantOcclusion(vec3 p, vec3 n, float d=1., int ite=6)
{
    float dlt = 0.0;
    float oc = 0.0;
    for(int i=1; i<=ite; i++)
    {
                dlt = d*pow(float(i)/float(ite),2.);
                oc += (dlt - scene(p+n*dlt))/exp(dlt);
    }
    oc /= float(ite)/2.;
    return clamp(1.0 - oc, 0.0, 1.0);
}


vec4 getColor(vec3 p, vec3 n, vec3 org, vec3 dir, float pass)
{
        vec3 lightdir = normalize(vec3(.75,1.0,0.25));

        float diffuse = max( dot(n,lightdir), 0.0)*0.5+0.5;
        float ao = getAmbiantOcclusion(p,n,4.,12);
        //float ao = getAO(p,n,vec2(0.5,1.5));

        vec4 color = vec4(0.9,0.95,1.0,1.0) * (diffuse*.8 + getAmbiantOcclusion(p,dir,1.,6)*.3 ) * ao ;
        if(abs(p.x)<5. && p.y<1. && p.z < 5.025)
                color = color*.25+vec4(0.1,1.,0.5,1.)*texture1D(spectrum,length(p.xz)/5.).x*(1.-p.y);
        if(abs(p.z)<5. && p.y<1. && p.x < 5.025)
                color = color*.25+vec4(0.1,1.,0.5,1.)*texture1D(spectrum,length(p.xz)/5.).x*(1.-p.y);
        color = mix(color, vec4(0.5), min(distance(p,org)*0.01,1.0)); //Fog

        return color;
}

//-----------------------------------------------------------------------------
// Main Loops
//-----------------------------------------------------------------------------
void main()
{

        vec3 org = vec3(5.0+cos(time*.1)*3.,3.+cos(time*.15)*cos(time*.1)*2.,10.0-cos(time*.1)*3.);
        vec3 dir = normalize(vec3(v.x*(16./9.),v.y+.2,-2.));
        dir = rotateX(dir,-.4-cos(time*.15)*cos(time*.1)*.25);
        dir = rotateY(dir,-0.4-cos(time*.1)*.5);
        float pass;
        vec3 p = raymarche(pass,org,dir,48,0.001);
        vec3 n = getNormal(p);
        vec4 color = getColor(p,n,org,dir, pass);


        dir = reflect(dir, n );
        p = raymarche(pass,p+dir*0.1,dir,32,0.001);
        color = mix(color, getColor(p,getNormal(p),org,dir,pass), .25);


        color *= vec4(.9,.95,1.,1.);

        gl_FragColor = color;

}





