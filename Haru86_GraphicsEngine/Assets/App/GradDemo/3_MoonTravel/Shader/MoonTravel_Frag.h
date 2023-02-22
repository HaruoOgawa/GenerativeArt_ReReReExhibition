#version 330
uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;

uniform float _ShowStartTime;
uniform float _ShowTimeDuration;
uniform float _Alpha;
uniform float _MoveSpeed;
uniform int _DrawEarth;

in vec2 uv;

// Shared Preprocessor ////////////////////////////////////////////////////////
#define pi 3.14159265
#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define dmin 0.001
#define tmax 30.0
#define ldir normalize(vec3(0.0,1.0,-1.0))

// Gloabal Valiable
vec3 g_ro;
int CameraIndex;
float ln; // LoopNum

// Useful Function ////////////////////////////////////////////////////////////

struct mapr // MapResult
{
   float d; // Distance
   bool  hit;
   int   m; // MaterialType
   float tvalue;     
   float i;
   float acc;
};

void compm(inout mapr mr,float d,int mt,bool IsMin) // CompareMap
{
    if(IsMin)
    {
        if(d<mr.d) mr = mapr(d,false,mt,0.0,0.0,0.0);
        if(mr.d<dmin) mr.hit=true; 
    }
    else
    {
        if(d>mr.d) mr = mapr(d,false,mt,0.0,0.0,0.0);
        if(mr.d<dmin) mr.hit=true; 
    }
}

vec3 trs(vec3 p,vec3 s,vec3 r,vec3 tvalue)
{
    p+=tvalue; 
    p.yz*=rot(s.x);p.xz*=rot(s.y);p.xy*=rot(s.z);
    p*=s;
    
    return p;
}

// Noise Function /////////////////////////////////////////////////////////////
float rand(in vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Distance Function
#define Moon_FREQ 0.16
#define Moon_HEIGHT 0.6
#define Moon_CHOPPY 4.8
#define ITER_GEOMETRY 3
#define ITER_FRAGMENT 3
#define Moon_SPEED 0.8
#define Moon_TIME (1.0+_time*Moon_SPEED)
#define NUM_STEPS 8
#define EPSILON_NRM (0.1 / _resolution.x)
#define Moon_BASE vec3(0.1, 0.19, 0.22)
#define Moon_WATER_COLOR vec3(0.8,0.9,0.6)

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

float Moon_octave(vec2 uv,float choppy)
{
    uv+=noise(uv);
    vec2 wv=1.0-abs(sin(uv));
    vec2 swv=abs(cos(uv));
    wv=mix(wv,swv,wv);
    
    return pow(1.0-pow(wv.x*wv.y,0.65),choppy);
}

mapr Moonmap(vec3 p)
{
    p.y += 0.5;

    mapr mr = mapr(1000.0,false,-1,0.0,0.0,0.0);

    float freq = Moon_FREQ;
    float amp = Moon_HEIGHT;
    float choppy = Moon_CHOPPY;
    
    vec2 uv = p.xz;
    
    float d,h=0.0;
    
    mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);
    
    // FBM
    for(int i=0;i<ITER_GEOMETRY;i++)
    {
        d=Moon_octave((uv)*freq,choppy);
        d+=Moon_octave((uv)*freq,choppy);
        h+=d*amp;
        
        uv*=octave_m;
        freq*=2.0;
        amp*=0.2;
        
        choppy=mix(choppy,1.0,0.2);
    }
    
    compm(mr, p.y-h, 1, true);
    
    return mr;
}

mapr Moonmap_detailed(vec3 p)
{
    p.y += 0.5;

    mapr mr = mapr(1000.0,false,-1,0.0,0.0,0.0);

    float freq = Moon_FREQ;
    float amp = Moon_HEIGHT;
    float choppy = Moon_CHOPPY;
    
    vec2 uv = p.xz;
    
    float d,h=0.0;
    
    mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);
    
    // FBM
    for(int i=0;i<ITER_FRAGMENT;i++)
    {
        d=Moon_octave((uv)*freq,choppy);
        d+=Moon_octave((uv)*freq,choppy);
        h+=d*amp;
        
        uv*=octave_m;
        freq*=2.0;
        amp*=0.2;
        
        choppy=mix(choppy,1.0,0.2);
    }
    
    compm(mr, p.y-h, 1, true);
    
    return mr;
}

mapr CreateMoonHightMap(vec3 ro,vec3 rd)
{
    ro.z += _time * _MoveSpeed; 

    float tm=0.0;
    float tx=1000.0;
    float hx=Moonmap(ro+rd*tx).d;
    mapr mr = mapr(1000.0,false,-1,0.0,0.0,0.0);
    float i=0.0,acc=0.0;
    //
    if(hx>0.0)
    {
        mr.hit = false;
        return mr;
    }
    
    float hm=Moonmap(ro+rd*tm).d;
    float tmid=0.0;
    
    for(i=0.0;i<float(NUM_STEPS);i++)
    {
        float f = hm/(hm-hx);
        tmid = mix(tm,tx,f);
        vec3 p=ro+rd*tmid;
        
        mr=Moonmap(p);
        acc += exp(-3.0 * mr.d);
        float hmid = mr.d;
        
        if(hmid<0.0)
        {
            tx=tmid;
            hx=hmid;
        }
        else
        {
            tm=tmid;
            hm=hmid;
        }
    }
    
    mr.i=i;mr.tvalue=tmid;mr.acc=acc;mr.hit = true;
    return mr;
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

// Ray Function ///////////////////////////////////////////////////////////////
vec4 fbm(vec4 p) {
	p.xz = abs(p.xz+1.)-abs(p.xz-1.)-p.xz;
	p=p*2./clamp(dot(p.xyz,p.xyz),.15,1.)-vec4(0.5,0.5,0.8,0.);
	p.xy*=rot(.5);
	return p;
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

mapr map(vec3 p)
{
    mapr mr = mapr(1000.0,false,-1,0.0,0.0,0.0);
    if(_time < _ShowStartTime)
    {
        compm(mr,length( trs(p,vec3(1.0),vec3(0.0),vec3(0.0,-2.0,0.0)) )-1.25,1,true);
    }
    else
    {
        p.z += _time * _MoveSpeed; 
        vec4 lp = vec4(p,1.5);
        lp.x = -abs(lp.x);
        lp.x = mod(lp.x, 2.0) -1.0;
        lp.x += 1.0;
        
        if(_time >= _ShowStartTime && _time < _ShowStartTime + _ShowTimeDuration)
        {
            float h = 8.0;
            lp.y += (h - h * (_time-_ShowStartTime)/_ShowTimeDuration);
        }
        
        lp.y += -1.5;
        lp.z=abs(2.0-mod(lp.z,4.0));
        for(int i=0;i<8;i++){lp = fbm(lp);}
        
        float d=length(max(vec2(0.0), lp.yz-3.0))/lp.w;
        compm(mr,d,0,true);
    }
    
    return mr;
}

vec3 gn(vec3 p)
{
    vec2 e=vec2(0.001,0.0);
    return normalize(vec3(
        map(p+e.xyy).d-map(p-e.xyy).d,
        map(p+e.yxy).d-map(p-e.yxy).d,
        map(p+e.yyx).d-map(p-e.yyx).d
    ));
}

vec3 gn_Moon(vec3 p,float eps)
{
    vec3 n;
    n.y=Moonmap_detailed(p).d;
    n.x=Moonmap_detailed(vec3(p.x+eps, p.y, p.z)).d-n.y;
    n.z=Moonmap_detailed(vec3(p.x, p.y, p.z+eps)).d-n.y;
    n.y=eps;
    
    return normalize(n);
}

mapr ray(vec3 ro,vec3 rd,const bool IsRef)
{
    mapr mr;
    
    //if(_time < _ShowStartTime) {return mapr(1000.0,false,-1,0.0,0.0,0.0);}
    
    float i=0.0,tvalue=0.0,acc=0.0;
    for(;++i<150.0;){mr=map(ro+rd*(tvalue+=mr.d*0.5));if(mr.d<dmin||mr.tvalue>tmax)break;acc+=exp(-3.0*mr.d);}
    
    mr.i=i;mr.tvalue=tvalue;mr.acc=acc;
    
    return mr;
}

float shadow(vec3 ro, vec3 rd)
{
    float sh=1.0;
    float tvalue=dmin;
    
    for(int i=0; i<32; i++)
    {
        float d=map(ro+rd*tvalue).d;
        if(sh<0.0001 || tvalue>tmax) break;
        sh=min(sh, max(0.0, 10.0*d/tvalue));
        tvalue+=d;
    }
    
    sh=clamp(sh, 0.0, 1.0);
    
    return sh;
}

float calcAO( const vec3 pos, const vec3 nor ) {
	float aodet=0.00004*40.;
	float totao = 0.0;
    float sca = 13.0;
    for( int aoi=0; aoi<5; aoi++ ) {
        float hr = aodet*float(aoi*aoi);
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos ).d;
        totao += -(dd-hr)*sca;
        sca *= 0.7;
    }
    return clamp( 1.0 - 5.0*totao, 0., 1.0 );
}

float calcAo(in vec3 p,in vec3 n)
{
    float k=1.0, occ=0.0;
    for(int i=0; i<5; i++)
    {
        float len = 0.15 + float(i) * 0.15;
        float d = map(n*len + p).d;
        occ += (len - d) * k;
        k*=0.5;
    }
    
    return clamp(1.0-occ, 0.0, 1.0);
}

// https://www.shadertoy.com/view/XlfGRj
#define tile   0.850
vec3 StarSpace(vec3 ro,vec3 rd)
{
    if(_time >= _ShowStartTime){ ro.z += _time * _MoveSpeed * 0.05; } 
    else{ ro.z += _time * _MoveSpeed * 0.001; } 

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

vec3 dColor(mapr mr, mapr moonr, vec3 ro,vec3 rd, vec2 st)
{
    vec3 col = vec3(0.0);
    
    if(mr.tvalue < 10.0 /*(mr.tvalue < moonr.tvalue || !moonr.hit) && mr.hit*/ && mr.m == 0)
    {
        vec3 p = ro + rd*mr.tvalue;
        vec3 n = gn(p);
        
        {
            //
            float sh=shadow(p, ldir);
            float diff=max(0.0, dot(ldir, n)) * sh;
            float spec=pow(max(0.0, dot(rd,-reflect(ldir,n))), 15.0);
            float ao=calcAO(p, n);
            col=ao * vec3(1.0,0.8,0.6)*diff + vec3(1.0,0.8,0.6)*spec;

            //
            vec3 refCol=StarSpace(p, reflect(rd, n));
            col=mix(col, refCol, 0.2);
        }
    }
    else if(!moonr.hit && mr.hit && mr.m == 1 && _DrawEarth == 1) // 地球のライティング
    {
            vec3 p = ro+rd*mr.tvalue;
            //p.xz*=rot(_time*0.5);
            vec3 n = gn(p);
            vec3 pn = n*0.5+0.5;
            
            // landBlend
            const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );
            vec3 q = p;
            q = m *q *2.01;
            
            // 大陸
            {
                float f = fbm3(2.0*q+vec3(160.0,40.0,120.0),6.0,1.0);
                f=pow(f,1.5);
                float landBlend = min(4.0*max(f-0.3,0.0),1.0);
                vec3 landCol = vec3(1.0,1.3,0.5);
                float continent = fbm3(5.0*q+vec3(120.0),3.0,1.0);
                vec3 oceanCol = vec3(0.0,0.031,0.11);
                col = mix(oceanCol,continent*landCol,landBlend);
            }
            
            // 雲
            {
                float f = fbm3(7.5*q+vec3(500)+_time,6.0,1.0);
                f=pow(f,1.5);
                float cloudBlend = min(4.0*max(f-0.3,0.0),1.0);
                vec3 cloudCol = vec3(1.0);
                float cloud = fbm3(7.5*q+vec3(500.0)+_time,3.0,1.0);
                vec3 oceanCol = vec3(0.0,0.031,0.11);
                col = mix(col,cloud*cloudCol,cloudBlend);
            }
    }
    else if(moonr.hit)
    {
        ro.z += _time * _MoveSpeed;

        vec3 p = ro + rd * moonr.tvalue;
        vec3 d = p - ro;
        vec3 n = gn_Moon(p,dot(d, d) * EPSILON_NRM);
        col = vec3(1.0)*max(0.0,dot(n,ldir));
        float ao = calcAo(p,n);
        col += ao*vec3(1.0)*0.1;
    }
    else
    {
        col=StarSpace(ro, rd);
    }
    
    return col;
}

// Main ///////////////////////////////////////////////////////////////////////
void main()
{
if(_RenderingTarget==2) // ZTest
{
    gl_FragColor = vec4(vec3(0.0),1.0);
}
else
{
    vec2 st=uv*2.0-1.0;st.x*=(_resolution.x/_resolution.y);
    vec3 ro= _WorldCameraPos,ta=_WorldCameraCenter;
    
    vec3 cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),cup=normalize(cross(cdir,cside)),
    rd=normalize(st.x*cside+st.y*cup+1.0*cdir),col=vec3(0.0);
    ln = 100.0;
    mapr mr = ray(ro, rd, false); mapr moonr = CreateMoonHightMap(ro, rd);
    col = dColor(mr,moonr,ro,rd,st);
    
    gl_FragColor = vec4(col,_Alpha);
}

}


