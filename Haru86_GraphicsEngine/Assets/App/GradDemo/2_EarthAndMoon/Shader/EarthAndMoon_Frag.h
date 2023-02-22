#version 330
uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;

uniform float _MoveVal;

in vec2 uv;

struct mapr // MapResult
{
   float d; // Distance
   int   m; // MaterialType
            // 0: Earth
};

void compm(inout mapr mr,float d,int mt,bool IsMin) // CompareMap
{
    if(IsMin)
    {
        if(d<mr.d) mr = mapr(d,mt);
    }
    else
    {
        if(d>mr.d) mr = mapr(d,mt);
    }
}

//////////////////////////

#define pi 3.14159265
#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))

// Get random value
float random(in vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Get noise
float noise(in vec2 st)
{
    // Splited integer and float values.
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i + vec2(0.0, 0.0));
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // -2.0f^3 + 3.0f^2
    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float hash( float n ) { return fract(sin(n)*753.5453123); }
float noise3( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}

float fbm(vec2 p)
{
    mat2 m=rot(35.6*pi/180.0);
    float f=0.0,ASum=0.0;
    for(float i=0.0;i<4.0;i++)
    {
        float Att = pow(0.5,i+1.0) ;
        f+=Att*noise(p);p=2.0*m*p;
        ASum+=Att;
    }
    
    return f/ASum;
}

float fbm3(vec3 p,float num,float A)
{
    float w=0.0,asum=0.0;
    for(float i=0.0;i<num;i++)
    {
        float Att = pow(0.5,i+1.0) * A;
        w+=Att*noise3(p);
        p=2.0*p;
        asum+=Att;
    }
    
    return w/asum;
}

vec3 trs(vec3 p,vec3 s,vec3 r,vec3 tvalue)
{
    p+=tvalue; 
    p.yz*=rot(s.x);p.xz*=rot(s.y);p.xy*=rot(s.z);
    p*=s;
    
    return p;
}

mapr map(vec3 p)
{
    mapr mr;
    mr.d=1000.0;
    mr.m=-1;
    
    {
        compm
        (
            mr,
            length( trs(p,vec3(1.0),vec3(0.0),vec3(0.0)) )-1.0,
            0,true
        );
    }
    
    {
        float r=3.5,h=0.95;
        compm
        (
            mr,
            length( 
                trs(p,vec3(1.0),vec3(0.0),
                    vec3(r*cos(5.451),0.85*h,r*sin(5.451))
                ) 
            )-0.5,
            1,true
        );
    }
    
    return mr;
}

// https://www.shadertoy.com/view/XlfGRj
#define tile   0.850
vec3 StarSpace(vec3 ro,vec3 rd)
{
    vec3 col=vec3(0.);
    float tvalue=0.1,fade=1.;
    ro.x+=1.0;
    //ro.z=mod(ro.z+_time*0.1,6.0)-3.0;
    
    for(int m=0;m<20;m++)
    {
        vec3 p = ro+rd*tvalue; float r=0.,SumD=0.;
        p = abs(vec3(tile)-mod(p,vec3(tile*2.))); // tiling fold
        // IFS
        for(int n=0;n<17;n++)
        {
            float k=2.0;
            p=abs(p)/dot(p,p)*k-0.5*k;
            SumD+=abs(length(p)-r);
            r=length(p);
        }
        SumD*=SumD*SumD;
        col+=vec3(tvalue,tvalue*tvalue,tvalue*tvalue*tvalue*tvalue)*SumD*0.0015*fade;
        fade*=0.730;
        tvalue+=0.1;
    }
    
    return col*0.01;
}

#define dmin 0.001
#define ln 64.0
#define ldir normalize(vec3(1.0,1.0,0.85))

vec3 gn(vec3 p)
{
    vec2 e = vec2(dmin,0.0);
    return normalize(vec3(
        map(p+e.xyy).d-map(p-e.xyy).d,
        map(p+e.yxy).d-map(p-e.yxy).d,
        map(p+e.yyx).d-map(p-e.yyx).d
    ));
}

void main()
{
if(_RenderingTarget==2) // ZTest
{
    gl_FragColor = vec4(vec3(0.0),1.0);
}
else
{
    //vec2 st = (gl_FragCoord.xy*2.0-_resolution.xy)/min(_resolution.x,_resolution.y);
    vec2 st=uv*2.0-1.0;
    st.x*=(_resolution.x/_resolution.y);
    float r=0.75,offset=0.25,h=0.95,tof=-1.1+_MoveVal;  /*tof=mod(_time*0.1,1.0)-0.5;*/
    vec3 col = vec3(0.0),
        ro=vec3(r*cos(1.71+tof),h,r*sin(1.71+tof)),
        ta=vec3(r*cos(1.71+offset+tof),0.85*h,r*sin(1.71+offset+tof)), 
    cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),cup=normalize(cross(cdir,cside)),
    rd=normalize(st.x*cside+st.y*cup+1.0*cdir);
    float i=0.0,tvalue=0.0,acc=0.0; mapr mr;
    
    for(;++i<ln;){mr=map(ro+rd*(tvalue+=mr.d));if(mr.d<dmin)break;acc+=exp(-3.0*mr.d);}

    // ”wŒi‚Ì‰F’ˆ
    col = StarSpace(ro,rd);
    
    //
    vec3 mooncol = pow(vec3(1.0),vec3(2.2));
    col += mooncol*( (mr.d<dmin)? 1.0 : acc )*0.02;
    //col += mooncol*acc*0.2;
    
    // ’n‹…‚Ì•`‰æ
    if(mr.m==0)
    {
        vec3 p = ro+rd*tvalue;
        //p.xz*=rot(_time*0.5);
        vec3 n = gn(p);
        vec3 pn = n*0.5+0.5;
            
        // landBlend
        const mat3 m = mat3( 0.00,  0.80,  0.60,
                -0.80,  0.36, -0.48,
                -0.60, -0.48,  0.64 );
        vec3 q = p;
        q = m *q *2.01;
        
        // ‘å—¤
        {
            float f = fbm3(2.0*q+vec3(160.0,40.0,120.0),6.0,1.0);
            f=pow(f,1.5);
            float landBlend = min(4.0*max(f-0.3,0.0),1.0);
            vec3 landCol = vec3(1.0,1.3,0.5);
            float continent = fbm3(5.0*q+vec3(120.0),6.0,1.0);
            vec3 oceanCol = vec3(0.0,0.031,0.11);
            col = mix(oceanCol,continent*landCol,landBlend);
        }
        
        // ‰_
        {
            float f = fbm3(7.5*q+vec3(500)+_time,6.0,1.0);
            f=pow(f,1.5);
            float cloudBlend = min(4.0*max(f-0.3,0.0),1.0);
            vec3 cloudCol = vec3(1.0);
            float cloud = fbm3(7.5*q+vec3(500.0)+_time,6.0,1.0);
            vec3 oceanCol = vec3(0.0,0.031,0.11);
            col = mix(col,cloud*cloudCol,cloudBlend);
        }

        //float diffuse = max(0.1,dot(n,ldir));
        //col*=diffuse;
   
    }
    else if(mr.m==1)
    {
        vec3 p = ro+rd*tvalue;
        vec3 n = gn(p);
        vec3 pn = n*0.5+0.5;
        col = vec3(1.0-fbm3(5.0*pn,5.0,1.0) )+vec3(0.5,0.4,0.15);
    }
    
    gl_FragColor = vec4(col,1.0);
}

}

