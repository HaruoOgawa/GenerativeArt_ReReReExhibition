

#version 330

uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;

uniform float _LeaveStartTime;
uniform int _CameraID;

in vec2 uv;

struct mapr // MapResult
{
   float d; // Distance
   int   m; // MaterialType
            // 0: Moon
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
float hash(vec2 p)
{
	float h = dot(p, vec2(127.1, 311.7));	
    return fract(sin(h) * 43758.5453123);
}

// Get noise
float noise(in vec2 st)
{
    // Splited integer and float values.
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = hash(i + vec2(0.0, 0.0));
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    // -2.0f^3 + 3.0f^2
    vec2 u = f * f * (3.0 - 2.0 * f);

    float result = mix(mix(a, b, u.x),
                        mix(c, d, u.x), u.y);
    
    // Normalized to '-1 - 1'.
    return (2.0 * result) - 1.0;
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

float hash(vec3 p)
{
    p=50.0*fract( p*0.3183099 + vec3(0.71,0.113,0.419));
    return -1.0+2.0*fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

vec4 noised(in vec3 x)
{
    vec3 p=floor(x);
    vec3 w=fract(x);
    
    vec3 u=w*w*w*(w*(w*6.0-15.0)+10.0);
    vec3 du=30.0*w*w*(w*(w-2.0)+1.0);
    
    float a = hash( p+vec3(0,0,0) );
    float b = hash( p+vec3(1,0,0) );
    float c = hash( p+vec3(0,1,0) );
    float d = hash( p+vec3(1,1,0) );
    float e = hash( p+vec3(0,0,1) );
    float f = hash( p+vec3(1,0,1) );
    float g = hash( p+vec3(0,1,1) );
    float h = hash( p+vec3(1,1,1) );
    
    float k0 = a;
    float k1 = b-a;
    float k2 = c-a;
    float k3 = e-a;
    float k4 = a-b-c+d;
    float k5 = a-c-e+g;
    float k6 = a-b-e+f;
    float k7 =-a+b+c-d+e-f-g+h;
    
    return vec4( -1.0+2.0*(k0 + k1*u.x + k2*u.y + k3*u.z + k4*u.x*u.y + k5*u.y*u.z + k6*u.z*u.x + k7*u.x*u.y*u.z),
                 2.0* du * vec3( k1 + k4*u.y + k6*u.z + k7*u.y*u.z,
                                 k2 + k5*u.z + k4*u.x + k7*u.z*u.x,
                                 k3 + k6*u.x + k5*u.y + k7*u.x*u.y ) );
}

const mat3 m3  = mat3( 0.00,  0.80,  0.60,
                      -0.80,  0.36, -0.48,
                      -0.60, -0.48,  0.64 );
const mat3 m3i = mat3( 0.00, -0.80, -0.60,
                       0.80,  0.36, -0.48,
                       0.60, -0.48,  0.64 );

vec4 fbm2(in vec3 x,int octaves)
{
    float f=1.9;
    float s=0.55;
    float a=0.0;
    float b=0.5;
    vec3 d=vec3(0.0);
    mat3 m=mat3(
        1.0,0.0,0.0,
        0.0,1.0,0.0,
        0.0,0.0,1.0
    );
    
    for(int i=0;i<octaves;i++)
    {
        vec4 n=noised(x);
        a+=b*n.x;
        d+=b*m*n.yzw;
        b*=s;
        x=f*m3*x;
        m=f*m3i*m;
    }
    
    a=a*0.5+0.5;
    
    return vec4(a,d);
}

vec3 trs(vec3 p,vec3 s,vec3 r,vec3 tvalue)
{
    p+=tvalue; 
    p.yz*=rot(s.x);p.xz*=rot(s.y);p.xy*=rot(s.z);
    p*=s;
    
    return p;
}

//////////////////////////
// https://qiita.com/edo_m18/items/a575606a60b21f0d2c57
// https://www.shadertoy.com/view/Ms2SD1

#define SEA_FREQ 0.16
#define SEA_HEIGHT 0.6
#define SEA_CHOPPY 4.0
#define ITER_GEOMETRY 3
#define ITER_FRAGMENT 5
#define SEA_SPEED 0.8
#define SEA_TIME (1.0+_time*SEA_SPEED)
#define NUM_STEPS 8
#define EPSILON_NRM (0.1 / _resolution.x)
#define SEA_BASE vec3(0.1, 0.19, 0.22)
#define SEA_WATER_COLOR vec3(0.8,0.9,0.6)

float sea_octave(vec2 uv,float choppy)
{
    uv+=noise(uv);
    vec2 wv=1.0-abs(sin(uv));
    vec2 swv=abs(cos(uv));
    wv=mix(wv,swv,wv);
    
    return pow(1.0-pow(wv.x*wv.y,0.65),choppy);
}

float seamap(vec3 p)
{
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    
    vec2 uv = p.xz;
    
    float d,h=0.0;
    
    mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);
    
    // FBM
    for(int i=0;i<ITER_GEOMETRY;i++)
    {
        d=sea_octave((uv+SEA_TIME)*freq,choppy);
        d+=sea_octave((uv-SEA_TIME)*freq,choppy);
        h+=d*amp;
        
        uv*=octave_m;
        freq*=2.0;
        amp*=0.2;
        
        choppy=mix(choppy,1.0,0.2);
    }
    
    return p.y-h;
}

float seamap_detailed(vec3 p)
{
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    
    vec2 uv = p.xz;
    
    float d,h=0.0;
    
    mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);
    
    // FBM
    for(int i=0;i<ITER_FRAGMENT;i++)
    {
        d=sea_octave((uv+SEA_TIME)*freq,choppy);
        d+=sea_octave((uv-SEA_TIME)*freq,choppy);
        h+=d*amp;
        
        uv*=octave_m;
        freq*=2.0;
        amp*=0.2;
        
        choppy=mix(choppy,1.0,0.2);
    }
    
    return p.y-h;
}

float CreateSeaHightMap(vec3 ro,vec3 rd,out vec3 p,inout bool IsSea)
{
    float tm=0.0;
    float tx=1000.0;
    float hx=seamap(ro+rd*tx);
    
    //
    if(hx>0.0)
    {
        p = vec3(0.0);
        IsSea=false;
        return tx;
    }
    
    float hm=seamap(ro+rd*tm);
    float tmid=0.0;
    
    for(int i=0;i<NUM_STEPS;i++)
    {
        float f = hm/(hm-hx);
        tmid = mix(tm,tx,f);
        p=ro+rd*tmid;
        
        float hmid=seamap(p);
        
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
    
    IsSea=true;
    return tmid;
}

mapr map(vec3 p)
{
    mapr mr;
    mr.d=1000.0;
    mr.m=-1;
    
    {
        float h=clamp(_time-_LeaveStartTime,0.0,1.0);
        compm(mr,length( trs(p,vec3(1.0),vec3(0.0),vec3(0.0,-0.35-h - 3.5,0.0)) )-0.5,0,true);
    }
    
    return mr;
}

// https://www.shadertoy.com/view/MlfXWH
#define EPS (2.0/_resolution.x)
vec3 sky(vec3 rd,bool IsRef)
{
    //
    vec3 col = vec3(0.0),skycol=mix(vec3(0.0,0.038,0.038),vec3(0.0,0.04,0.15),(rd.y*0.5+0.5));
 
    // êØ
    vec3 star = vec3( smoothstep(0.8,0.95,fbm((100.0*rd.xy)/rd.z)) ); 
    
    // â_
    vec3 clouds = vec3(0.0);
    float s = 0.25;
    for(int i=0;i<3;i++)
    {
        clouds+=fbm(rd.xz/(rd.y)-s+vec2(0.0,_time*0.5));
        s*=1.35;
    }
    
    //
    col = skycol + 0.25*clouds*max(0.0,rd.y);
    col += star*max(0.0,rd.y)*2.0; 
    
    if(IsRef)
    {
        // åé
        vec2 moonPos = rd.xy/rd.z+vec2(0.0,0.25);
        float moond=length(moonPos),moonr=0.355;
        vec3 moonCol = vec3(fbm(moonPos*10.0))
            *smoothstep(0.37,0.35,moond);
        moonCol+=vec3(1.0)*pow(moond*2.25,-16.0)
        *( (moond<moonr)? 0.0:0.01 );
        
        col += moonCol*vec3(0.5,0.4,0.15)*2.0;
    }
    
    return col;
}

#define dmin 0.001
#define ln 64.0
#define ldir normalize(vec3(1.0,1.0,-1.0))

vec2 CloudMap(vec3 p,int num,inout vec3 denGra)
{
    float f = fbm2(p*0.5+vec3(-_time*0.25,0.0,0.0),num).x;
    f=smoothstep(-0.2,0.6,f);
    
    float d = p.y - f*0.1;
    
    denGra = vec3(0.0, sign(p.y), 0.0);
    
    return vec2(d,f);
}

vec3 getSeaColor(vec3 p,vec3 n,vec3 l,vec3 eye,vec3 dist)
{
    // îΩéÀÇ∆ã¸ê‹
    float fresnel = clamp(1.0-dot(n,-eye),0.0,1.0);
    fresnel = pow(fresnel,3.0)*0.65;
    
    vec3 reflected=sky(reflect(eye,n),true);
    vec3 refracted=SEA_BASE+pow(dot(n,l)*0.4+0.6,80.0)*SEA_WATER_COLOR*0.12;
    
    vec3 col = mix(reflected,refracted,fresnel);
    
    //
    float atten=max(1.0-dot(dist,dist)*0.001,0.0);
    col+=SEA_WATER_COLOR*(p.y-SEA_HEIGHT)*0.18*atten;
    
    // Specular
    col+=vec3( pow(max(dot(reflect(eye,n),l),0.0),60.0)* ((60.0+8.0)/(pi*8.0)));
    
    
    return col;
}

vec4 DrawCloud(vec3 ro,vec3 rd,in vec4 col)
{;
    float d=0.0,tvalue=0.0,i=0.0,h=0.0;
    vec4 CloudCol = vec4(0.0);
    
    for(;++i<ln;i++)
    {
        vec3 p = ro+rd*tvalue;
        vec3 denGra;
        vec2 re=CloudMap(p, 5, denGra);
        d=re.x;
        h=re.y;
        
        if(d>dmin)
        {
            // Local Color
            vec4 lcol = vec4(vec3(mix(0.0,1.0,h)), h*1.2);
            lcol.a*=0.4;
            lcol.rgb*=lcol.a;

            //
             vec3 nor = normalize(denGra);
            float dif = clamp(dot(nor,ldir), 0.0, 1.0 )/**sha*/;
            lcol.rgb*=dif;
            // â_ÇÃçÇÇ≥í≤êÆ
            CloudCol+=lcol*(1.0-CloudCol.a) * clamp(1.3 - p.y * 0.2, 0.0, 1.0);
            
           
        }
        
        tvalue+=max(0.05,0.1*d);
    }
    
    CloudCol = max(vec4(0.0), CloudCol);
    col.rgb = mix(col.rgb, CloudCol.rgb + vec3(0.5), CloudCol.a);

    float rate = 1.0 - clamp(tvalue/30.0, 0.0, 1.0);
    vec3 skycol=mix(vec3(0.0,0.038,0.038),vec3(0.0,0.04,0.15),(mod(rd.y, 1.0)*0.5+0.5));
    col.rgb = mix(skycol, col.rgb, rate);

    return col;
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

vec3 gn_sea(vec3 p,float eps)
{
    vec3 n;
    n.y=seamap_detailed(p);
    n.x=seamap_detailed(vec3(p.x+eps, p.y, p.z))-n.y;
    n.z=seamap_detailed(vec3(p.x, p.y, p.z+eps))-n.y;
    n.y=eps;
    
    return normalize(n);
}

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
    vec2 st=uv*2.0-1.0;
    st.x*=(_resolution.x/_resolution.y);
    float h=max(0.0,_time-_LeaveStartTime/*+0.25*/),
    LeaveRate=clamp(_time-_LeaveStartTime,0.0,1.0);
    vec3 ro=_WorldCameraPos,ta=_WorldCameraCenter;
    ro.y+=3.5; ta.y+=3.5; 

    // ÉJÉÅÉâÉèÅ[ÉN
    if(LeaveRate>=1.0)
    {
        ro=vec3(0.0,h,1.5),ta=vec3(0.0,h,0.0);
        
        if(_CameraID == 0)
        {
            // down side camera
            ta+=2.0;
            ro.xz*=rot(_time);
        }
        else if(_CameraID == 1)
        {
            // default Camera
            ro = ro + _WorldCameraPos, ta = ta + _WorldCameraCenter;
        }
        else if(_CameraID == 2)
        {
            // upside camera
            ro+=vec3(2.0);
        }
    }
    
    //
    vec4 col = vec4(vec3(0.0), 1.0);
    vec3 cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),cup=normalize(cross(cdir,cside)),
    rd=normalize(st.x*cside+st.y*cup+1.0*cdir);
    float i=0.0,tvalue=0.0,acc=0.0; mapr mr;
    
    if(LeaveRate<1.0)
    {
        for(;++i<ln;){mr=map(ro+rd*(tvalue+=mr.d));if(mr.d<dmin)break;acc+=exp(-3.0*mr.d);}

        // îwåiÇÃñÈãÛ
        col.rgb = sky(rd,false);
        col.rgb = smoothstep(0.0,1.0,col.rgb);
        col.rgb*=1.2;

        //
        vec3 mooncol = pow(vec3(1.0),vec3(2.2));
        col.rgb += mooncol*( (mr.d<dmin)? 1.0 : acc )*0.02;
        //col += mooncol*acc*0.2;

        if(mr.m==0)
        {
            vec3 p = ro+rd*tvalue;
            p.y-=h;
            //vec3 n = gn(p);
            //vec3 pn = n*0.5+0.5;
            //col = vec3(1.0-fbm(5.0*pn.xy/pn.z) )+vec3(0.5,0.4,0.15);
            col.rgb = vec3(1.0-fbm3(7.5*p,4.0,1.0))+vec3(0.5,0.4,0.15);
        }

        // Sea
        //if(mr.d>dmin)
        {
            vec3 p;
            bool IsSea=false;
            CreateSeaHightMap(ro+vec3(0.0,(h>0.0)?h+=0.25:0.0,0.0),rd,p,IsSea);
            vec3 dist = p-ro;
            float seat = length(dist);
            vec3 n = gn_sea(p,dot(dist,dist)*EPSILON_NRM);

            if(IsSea)
            {
                col.rgb = vec3(1.0)*max(0.0,dot(n,ldir));
                vec3 SeaCol = getSeaColor(p,n,ldir,rd,dist);
                col.rgb=SeaCol;
            }
        }
    }
    
    // â_Ç∆êØ
    vec4 StarCloudCol = vec4(vec3(0.0), 1.0);
    if(_time >= _LeaveStartTime)
    {
        // 43sÇ©ÇÁÉXÉ^Å[Ég
        // 0s ~ 1s : è„è∏äJénÅBÇ‹ÇæSeaMoonÇ™å©Ç¶ÇƒÇ¢ÇÈ
        // 1s ~ 10s : CloudÇ∆StarSpaceÇÃê¸å`ï‚äÆ
        // 10s ~ 13s : StarSpaceÇæÇØÇï`âÊ 
        float cs_mixrate = clamp((_time-_LeaveStartTime-1.0-5.0)/9.0,0.0,1.0);
        vec4 CloudCol = DrawCloud(ro, rd, col);
        vec4 StarCol = vec4(StarSpace(ro,rd), 1.0);
        StarCloudCol = mix(CloudCol,StarCol,cs_mixrate);
    }
    
    //
    col = mix(col,StarCloudCol,LeaveRate);
    
    gl_FragColor = col;
}

}


