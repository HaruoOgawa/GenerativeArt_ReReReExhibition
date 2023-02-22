

#version 330

uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;
uniform int _IsUseShowing;
uniform float _ShowingFinTime;
uniform float _ShowDuration;

uniform float _MoveStartTime;
uniform float _MoveTimeDuration;

uniform float _MoveH;

uniform int _TRSIndex;
uniform float _GoMoonTime;
uniform int _RefMapIndex ; // 0 ~ -1
uniform sampler2D _BufferA;
uniform float _CorrectionValue;
uniform int _UseCustomPos;
uniform vec3 _CustomRo;
uniform vec3 _CustomTa;

in vec2 uv;

// Shared Preprocessor ////////////////////////////////////////////////////////
#define pi 3.14159265
#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define dmin 1.0
#define tmax 3000.0
#define ldir normalize(vec3(1.0,1.0,-1.0))

// Gloabal Valiable
vec3 g_ro;
int CameraIndex;
float ln; // LoopNum
vec3 g_ShipPos;

// Useful Function ////////////////////////////////////////////////////////////

float smin(float d1,float d2,float v)
{
    float h=exp(-v*d1)+exp(-v*d2);
    return -log(h)/v;
}

struct mapr // MapResult
{
   float d; // Distance
   bool  hit;
   int   m; // MaterialType
   float tvalue;     
   float i;
   float acc;
   vec3 jetCol;
   float metallic;
};

void compm(inout mapr mr,float d,int mt,bool IsMin, bool IsSMin) // CompareMap
{
    if(IsMin)
    {
        if(!IsSMin)
        {
            if(d<mr.d) mr = mapr(d,false,mt,0.0,0.0,0.0,vec3(0.0),0.0);
            if(mr.d<dmin) mr.hit=true; 
        }
        else
        {
            if(d<mr.d) mr = mapr(smin(d, mr.d, 1.5),false,mt,0.0,0.0,0.0,vec3(0.0),0.0);
            if(mr.d<dmin) mr.hit=true; 
        }
    }
    else
    {
        if(d>mr.d) mr = mapr(d,false,mt,0.0,0.0,0.0,vec3(0.0),0.0);
        if(mr.d<dmin) mr.hit=true; 
    }
}

vec3 trs(vec3 p,vec3 s,vec3 r,vec3 tvalue)
{
    p+=tvalue;
    p.yz*=rot(r.x);p.xz*=rot(r.y);p.xy*=rot(r.z);
    p*=s;
    return p;
}

////////////////////////////////////////////////////
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

// https://www.shadertoy.com/view/XlfGRj
#define tile   0.850
vec3 StarSpace(vec3 ro,vec3 rd)
{
    vec3 col=vec3(0.);
    float tvalue=0.1,fade=1.;
    ro.x+=1.0;
    
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

vec3 dRefColor(vec3 ro, vec3 rd, vec3 n){vec3 dir=reflect(-rd, n); return StarSpace(reflect(-rd, n), rd);}

// https://www.shadertoy.com/view/4tscR8
float mav(vec2 a){return max(a.y,a.x);}
float mav(vec3 a){return max(a.z,mav(a.xy));}
vec3 F(vec3 a, float b){float c=sin(b),d=cos(b);return mat3(d,-c,0,c,d,0,0,0,1)*a;}
vec3 H(vec3 a){a=F(a,(floor(atan(a.y,a.x)*1.5/acos(-1.))*2.+1.)*acos(-1.)/3.);
 return vec3(a.x,abs(a.y),a.z);}
float R(vec3 a){vec3 b=abs(a);return max(b.y,dot(vec3(.87,.5, 0), b))- 1.;}
float S(vec3 a){return max(max(abs(length(a-vec3(0,0,5.))-5.)-.05,R(a)),a.z-2.);}
float Q(vec3 a){return max(abs(length(a*vec3(1,1,.3))-.325)-.025,-a.z);}
float P(vec3 a){vec3 b=abs(a);
 return max(mav(b),max(max(length(b.xy),length(b.yz)),length(b.zx))-.2)-1.;}
float T(vec3 a){
 vec3 b=a*20.0,c=H(b*2.+vec3(0,0,2))-vec3(1.4,0,0),d=b;
 d.y=abs(d.y);
 return 
      min(
          min(
              min(max(R(d*4.-vec3(2,5,0))*.25,abs(d.z)-1.),S(d.yzx*vec3(1,.5,.5)*1.5 + vec3(.3,0,0))/1.5),
              max(min(.1-abs(d.x),-d.z),S(vec3(0, 0, 1) - d.xzy * vec3(1, .5, .5)))),
          min(
              min(max(P(c),-P(c * 1.2 + vec3(0,0, 1.5)) / 1.2),Q(c + vec3(0, 0, 1.5))),
              Q(vec3(abs(c.xy), c.z) - vec3(.5,.5,-1.5)))*.5)*.05;
}

// M : ”òs‘D–{‘Ì
mapr map(vec3 p)
{
    mapr mr = mapr(1000.0,false,-1,0.0,0.0,0.0,vec3(0.0),0.0);
    if(_TRSIndex == 0)
    {
        float rate = clamp((_time-_MoveStartTime)/_MoveTimeDuration, 0.0, 1.0);
        p = mix(
            trs(p, vec3(1.0), vec3(0.0, 0.0, pi*0.5), vec3(0.0)),
            trs(p,vec3(1.0), vec3(6.21*0.25, sin(_time)*0.15, sin(_time)*0.15+pi*0.5), vec3(0.0, -0.1, 0.0)),
            rate
        );
    }
    else if(_TRSIndex == 1)
    {
        vec3 mdir = vec3(0.0);
        if(_time>= _GoMoonTime) mdir.xz += (exp(_time-_GoMoonTime) - exp(0.001))*10.0;
        vec3 mdir_n = normalize(mdir); 
        float axz = 0.0;
         axz = mix(0.0, atan(-1.0, 1.0), clamp((_time-_GoMoonTime), 0.0, 1.0));
        p = trs(p,vec3(1.0), vec3(11.7*0.25,11.7*0.25 +pi+axz,pi*1.1 + sin(_time)*0.15+pi*0.5), mdir);
    }
    else
    {
        p = trs(p, vec3(1.0), vec3(0.0, sin(_time)*0.15, sin(_time)*0.15+pi*0.5),vec3(0.0));
    }
    
    float ShowingRate = clamp((_time - (_ShowingFinTime-_ShowDuration))/_ShowDuration, 0.0, 1.0);
    if(ShowingRate<1.0)
    {
        float disp = (ShowingRate <0.001)? 0.0 : fbm2(p*10.0+_time*pow(ShowingRate,2.0), 5).x*ShowingRate;
        compm(mr, mix(length(p)-0.12*min(1.0,ShowingRate*1.0)-disp*0.05, T(p), ShowingRate), 0, true, false);
        mr.metallic = mix(1.0, 0.5, ShowingRate);
    }
    else
    {
        compm(mr, T(p), 0, true, false);
        mr.metallic = 0.5;
    }
      
    return mr;
}

mapr ray(vec3 ro, vec3 rd, bool IsRef)
{
    float tvalue=0.0,i=0.0,acc=0.0;mapr mr;vec3 jetCol=vec3(0.0);
    for(i=0.0; i<80.0; ++i) {
        vec3 p = ro+rd*tvalue;
        mr = map(p);
        
        if(abs(mr.d)<(tvalue*5.0 + 1.0)*.0001 || tvalue>=3000.0) break;
        acc += exp(-3.0 * mr.d);
        tvalue = min(tvalue+mr.d, 3000.0);
        
        vec3 jDir = g_ShipPos - p;
        jetCol += exp(-1.0*length(jDir)) * vec3(0.33,0.33,0.88);
    }
    mr.i=i;mr.tvalue=tvalue;mr.acc=acc;mr.jetCol=jetCol;
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

void main()
{
    vec2 st=uv*2.0-1.0;st.x*=(_resolution.x/_resolution.y);
    vec3 ro= _WorldCameraPos,ta=_WorldCameraCenter;
    if(_MoveH > 0.0) { ro.y -= _MoveH; ta.y -= _MoveH; }
   ro *= _CorrectionValue;

   if(_UseCustomPos == 1)
   {
        ro = _CustomRo;
        ta = _CustomTa; 
   }
    vec3 cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),cup=normalize(cross(cdir,cside)),
    rd=normalize(st.x*cside+st.y*cup+1.0*cdir),col = vec3(0.0);
    g_ShipPos = vec3(0.0);
    mapr mr= ray(ro, rd, false);
    vec3 p = ro + rd * mr.tvalue;
    float Alpha = 0.0;

    if(mr.hit && mr.m == 0)
    {
         vec3 BaseCol = vec3(1.0);
         vec3 n = gn(p);
         vec3 nabs = abs(n);
         float diff = max(0.0, dot(nabs, ldir));
         float spec = pow(max(0.0, dot(reflect(-ldir, nabs), -rd)), 16.0);
         float ambient = 0.25;
         col = mr.jetCol*0.05 * (diff + ambient) + vec3(1.0) * spec;
         
         {
             ro = p;
             rd = reflect(rd, n);
             mapr ref_mr = ray(ro ,rd, true);
             vec3 refcol = dRefColor(ro, rd, n);
             col = mix(col, refcol, mr.metallic);
         }
         Alpha = 1.0;
    }
    
    gl_FragColor = vec4(col, Alpha);
}

