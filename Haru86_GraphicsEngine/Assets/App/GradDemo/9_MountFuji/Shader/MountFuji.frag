R"(

// "Mountain Peak" by Alexander Alekseev aka TDM - 2014
// License: https://www.shadertoy.com/view/llK3WR

#version 330
// ShaderToy --> OpenGL/GLSL Convert Preprocessor /////////////////////////////
//#define DRAW_ON_SHADERTOY

#ifdef DRAW_ON_SHADERTOY
#define _resolution iResolution
#define gl_FragCoord fragCoord
#define gl_FragColor fragColor
#define _time iTime
#define main() mainImage( out vec4 fragColor, in vec2 fragCoord )
const int _RenderingTarget = 1;
const float _LeaveStartTime = 1.0;
#else
uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;

uniform float _LeaveStartTime;

in vec2 uv;
#endif

// Shared Preprocessor ////////////////////////////////////////////////////////
#define pi 3.14159265
#define rot(a) mat2(cos(a),sin(a),-sin(a),cos(a))
#define dmin 0.0003
#define tmax 100.0
#define ln 128.0
#define ldir normalize(vec3(1.0,1.0,-1.0))

// Gloabal Valiable
vec3 g_CloudP;
float g_CloudT;

// Useful Function ////////////////////////////////////////////////////////////

struct mapr // MapResult
{
   float d; // Distance
   bool  hit;
   int   m; // MaterialType
            // 0: Debug
            // 1: Mountain
};

void compm(inout mapr mr,float d,int mt,bool IsMin) // CompareMap
{
    if(IsMin)
    {
        if(d<mr.d) mr = mapr(d,false,mt);
        if(mr.d<dmin) mr.hit=true; 
    }
    else
    {
        if(d>mr.d) mr = mapr(d,false,mt);
        if(mr.d<dmin) mr.hit=true; 
    }
}

vec3 trs(vec3 p,vec3 s,vec3 r,vec3 t)
{
    p+=t; 
    p.yz*=rot(s.x);p.xz*=rot(s.y);p.xy*=rot(s.z);
    p*=s;
    
    return p;
}

// Noise Function /////////////////////////////////////////////////////////////

float hash(vec2 p) {
    uint n = floatBitsToUint(p.x * 122.0 + p.y);
	n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return uintBitsToFloat( (n>>9U) | 0x3f800000U ) - 1.0;
}

float rand(vec2 p)
{
    return fract( sin(dot(p,vec2(12.9898,78.233)))*43758.5453123 );
}

float rand3(vec3 p)
{
    return fract( sin(dot(p,vec3(12.9898,78.233,128.852)))*43758.5453123 );
}

vec3 noise(in vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    vec2 u = f*f*(3.0-2.0*f);
    
    float a = hash(i+vec2(0.0,0.0));
    float b = hash(i+vec2(1.0,0.0));
    float c = hash(i+vec2(0.0,1.0));
    float d = hash(i+vec2(1.0,1.0));
    float h1 = mix(a,b,u.x);
    float h2 = mix(c,d,u.x);
    
    return vec3(
        abs(mix(h1,h2,u.y)),
        6.0*f*(1.0-f)*(vec2(b-a,c-a)+(a-b-c+d)*u.yx)
    );
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

// Distance Function
vec3 octave(vec2 uv)
{
    vec3 n = noise(uv+vec2(0.5,1.0));
    return vec3(pow(n.x,1.9),n.y,n.z);
}

float mountain(vec3 p)
{
    //p.xz*=0.75; // éRÇÃçLÇ™ÇËãÔçá
    //p.y*=2.5; // éRÇÃçÇÇ≥

    float frq=0.24,amp=1.0,h=0.0;
    vec2 uv=p.xz*frq+vec2(13.5,15.0),dsum=vec2(0.0);
    
    for(int i=0;i<7;i++)
    {
        vec3 n = octave((uv-dsum*0.7)*frq);
        h+=n.x*amp;
        dsum+=n.yz*(n.x*2.0-1.0)*amp;
        frq*=2.5;
        amp*=0.58;
        amp*=pow(n.x,0.27);
    }
    
    h*=7.0/(1.0+dot(p.xz,p.xz)*1e-2)+2.0/(1.0+dot(p,p)*1e-3);
    //h*=30.0/(1.0+dot(p.xz,p.xz)*1e-2)+2.0/(1.0+dot(p,p)*1e-3);
    // ÉJÉãÉfÉâÇçÏê¨
    //h=min(h,17.0);
    h=min(h,8.5);
    //h=min(h,35.0);
    
    return p.y-h;
}

// Ray Function ///////////////////////////////////////////////////////////////
mapr map(vec3 p)
{
    mapr mr;
    mr.d=1000.0;
    mr.hit=false;
    mr.m=-1;
    
    {compm(mr, mountain(p), 1, true);}
    
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


vec2 CloudMap(vec3 p,int num,inout vec3 denGra)
{
    float f = fbm2(p*0.5+vec3(-_time*0.25,0.0,0.0),num).x;
    f=smoothstep(-0.2,0.6,f);
    
    float d = p.y - f*0.1;
    
    denGra = vec3(0.0, sign(p.y), 0.0);
    
    return vec2(d,f);
}

// Lighting //////////////////////////
vec3 sky_color(vec3 e) {
    e.y = max(e.y,0.0);
    vec3 ret;
    ret.x = pow(1.0-e.y,3.0);
    ret.y = pow(1.0-e.y, 1.2);
    ret.z = 0.8+(1.0-e.y)*0.3;    
    return ret;
}

// raytrace ///////////////////////////////////
vec4 DrawCloud(vec3 ro,vec3 rd,in vec4 col)
{
    //col.rgb*=0.1;
    //col.a=0.0;
    float d=0.0,t=0.0,i=0.0,h=0.0;
    vec4 CloudCol = vec4(0.0);
    
    for(;++i<ln;i++)
    {
        vec3 p = ro+rd*t;
        vec3 denGra;
        vec2 re=CloudMap(p, 5, denGra);
        d=re.x;
        h=re.y;
        
        //if(d<dmin)break;
        //if(t>50.0)break;
        //if(t>5000.0)break;
        
        //if(d<dmin)
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
        
        //t+=d;
        t+=max(0.05,0.1*d);
    }
    
    /*float rate = 1.0 - clamp(t/100.0, 0.0, 1.0);
    col.rgb = mix(sky_color(rd),
        mix(col.rgb, CloudCol.rgb, CloudCol.a),
    rate); 
    col.rgb = smoothstep(0.0, 1.0, col.rgb);*/
    
    //CloudCol.rgb = exp(CloudCol.rgb)*0.35;
    
    CloudCol = max(vec4(0.0), CloudCol);
    //col.rgb *= 1.0 - CloudCol.a;
    //col += CloudCol;
    col.rgb = mix(col.rgb, CloudCol.rgb + vec3(0.5), CloudCol.a);
    //col.rgb = smoothstep(0.0, 0.5, col.rgb);

    float rate = 1.0 - clamp(t/30.0, 0.0, 1.0);
    col.rgb = mix(sky_color(rd), col.rgb, rate);

    g_CloudT = t;
    g_CloudP = ro+rd*t;
    
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
#ifdef DRAW_ON_SHADERTOY
    vec2 st=(gl_FragCoord.xy*2.-_resolution.xy)/min(_resolution.x,_resolution.y);
    vec3 ro=vec3(0.0,0.0,1.5),ta=vec3(0.0,0.0,0.0);
    float tStep = 45.5;
    ro=vec3(27.5*cos(tStep*0.1),3.5,27.5*sin(tStep*0.1));
#else
    vec2 st=uv*2.0-1.0;
    st.x*=(_resolution.x/_resolution.y);
    vec3 ro=_WorldCameraPos,ta=_WorldCameraCenter;
#endif
    vec4 col = vec4(vec3(0.0),1.0);
    vec3 cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),
    cup=normalize(cross(cdir,cside)),rd=normalize(st.x*cside+st.y*cup+1.*cdir);
    float i=0.0,t=0.0; mapr mr;
    
    for(;++i<ln;){mr=map(ro+rd*(t+=mr.d));if(mr.d<dmin)break;}

    if(mr.hit)
    {
        if(mr.m == 0)
        {
            //col.rgb = vec3(1.0)*10.0/i;
            col = vec4(vec3(exp(-0.1*t)), 1.0);
        }
        else if(mr.m == 1)
        {
            vec3 p = ro+rd*t;
            vec3 n = gn(p);
            
            float slope = 1.0-dot(n,vec3(0.0,1.0,0.0));
            float hcval=mix(1.0, slope*slope, clamp((p.y-5.0)/5.0,0.0,1.0));
            vec3 ret=mix(
                vec3(1.0,1.0,1.0)*2.2,
                vec3(0.0,0.0,0.1),
                smoothstep(0.0, 0.35, hcval)
            );
            
            float diff = max(0.1,dot(ldir,n));
            
            col.rgb=ret*vec3(0.5,0.59,0.75)*0.6;
            col.rgb+=vec3(0.0,0.04,0.15)*hcval;
            col.rgb+=vec3(1.0,1.0,0.98)*diff * min(1.0, 0.111*exp2(p.y) );
           
            // fog
            vec3 ramda=exp2(-0.0025*t*vec3(1.0,2.0,4.0));
            col.rgb=mix(vec3(0.5),col.rgb,ramda);
        }
    }
    else
    {
        // sky
        col.rgb=mix(col.rgb, sky_color(rd), step(dmin,mr.d));
    }
    
    // cloud
    vec4 CloudCol=DrawCloud(ro,rd,col);
    //col = CloudCol;
    //col = mix(col,CloudCol,clamp(CloudCol.a, 0.0, 1.0));
    
    float DepthM = t;
    float DepthC = g_CloudT*0.55;
    //col = (DepthM < DepthC || CloudCol.r<0.1)? col : CloudCol;
    float rate = clamp((DepthM-DepthC) , 0.0 , 1.0);
    col = mix(col , CloudCol, rate);
    
    gl_FragColor = col;
}

}

)"