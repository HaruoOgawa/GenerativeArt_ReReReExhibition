

// "Mountain Peak" by Alexander Alekseev aka TDM - 2014
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// https://www.shadertoy.com/view/mdfGzn

#version 330

uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;

uniform float _LeaveStartTime;
uniform float _Alpha;

in vec2 uv;

// Shared Preprocessor ////////////////////////////////////////////////////////
#define pi 3.14159265
#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define dmin 0.0003
#define tmax 30.0
#define ldir normalize(vec3(1.0,1.0,-1.0))

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
            // 0: Debug
            // 1: Common
            // 2: CyberPunk
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

vec3 trs(vec3 p,vec3 s,vec3 r,vec3 tvalue)
{
    p+=tvalue; 
    p.yz*=rot(s.x);p.xz*=rot(s.y);p.xy*=rot(s.z);
    p*=s;
    
    return p;
}

float modc(float a, float b){return a-b*floor(a/b);}

// Noise Function /////////////////////////////////////////////////////////////

// Distance Function
float cube(vec3 p,vec3 s)
{
    return length(max(vec3(0.0), abs(p)-s));
}

float PrCapsDf (vec3 p, float r, float h)
{
  return length (p - vec3 (0., 0., h * clamp (p.z / h, -1., 1.))) - r;
}
// https://www.shadertoy.com/view/MlfSWX
float Mandel(vec3 p)
{
    const float mScale =  2.5;
    const float dstFar = 30.0;
    
    vec4 q,q0;
    const int nIter=30;
    q0=vec4(p,1.0);
    q=q0;
    for(int n=0;n<nIter;n++)
    {
        q.xyz = clamp(q.xyz,-1.0,1.0)*2.0-q.xyz;
        q*=mScale/clamp(dot(q.xyz,q.xyz), 0.5, 1.0);
        q+=q0;
        q.xyz+=g_ro*0.1;
    }
    
    float d0 = length(q.xyz)/abs(q.w);
    //float d0 = cube(q.xyz,vec3(1.0))/abs(q.w);
    
    if(CameraIndex == 0)
    {
        float d1 = PrCapsDf(p-g_ro,0.2,0.5);
        return max(d0, -d1);
    }
    else
    {
        return d0;
    }
}

// Ray Function ///////////////////////////////////////////////////////////////
mapr map(vec3 p)
{
    mapr mr;
    mr.d=1000.0;
    mr.hit=false;
    mr.m=-1;
    
    //{compm(mr,length(trs(p,vec3(1.0),vec3(0.0),vec3(0.0)))-0.5,2,true);}
    {compm(mr, Mandel(p), 2,true);}
    //{compm(mr,1.0+p.y /*cube(p, vec3(0.5))*/, 0, true);}
    
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

// Lighting ///////////////////////////////////////////
vec3 hsv2rgb2(vec3 c, float k) {
    return smoothstep(0. + k, 1. - k,
        .5 + .5 * cos((vec3(c.x, c.x, c.x) + vec3(3., 2., 1.) / 3.) * radians(360.)));
}

// https://www.shadertoy.com/view/MsGGDK
vec3 DrawCyberpunk(vec3 n,float tvalue)
{
    vec3 h = vec3(1.0);
    
    vec3 c = fract(n*0.1);
    vec3 d = abs(0.5-c)-0.5;
    float b = 1.0,e = 0.0;
    
    for(int i=0;i<7;i++)
    {
        d = 0.5 - abs(0.5-fract(c*b))*3.0;
        b*=3.0;
        e = max(min(max(d.x, d.z), min(max(d.x, d.y), max(d.y, d.z))) / b, e);
    }
    
    b=smoothstep(-(tvalue*0.001+0.05), tvalue*0.001+0.05, abs(0.5-fract(e*900.0))-0.4);
    h=(b+0.01)*0.002*vec3(5.0,1.0,5.0);
    
    vec3 col = pow(h/(abs(h)+1.0), vec3(0.45));
    
    return col;
}
// https://github.com/i-saint/RaymarchingOnUnity5/blob/master/Assets/Raymarching/Raymarcher.shader
vec2 DrawPattern(vec2 p)
{
    p=fract(p);
    float r = 0.123;
    float v=0.0,g=0.0;
    r=fract(r*9184.928);
    float cp,d;
    
    d=p.x;
    g+=pow(clamp(1.0-abs(d), 0.0, 1.0), 1000.0);
    d=p.y;
    g+=pow(clamp(1.0-abs(d), 0.0, 1.0), 1000.0);
    d=p.x - 1.0;
    g+=pow(clamp(3.0-abs(d), 0.0, 1.0), 1000.0);
    d=p.y - 1.0;
    g+=pow(clamp(1.0-abs(d), 0.0, 1.0), 10000.0);
    
    const int ITER = 12;
    for(int i=0; i<ITER; i++)
    {
      cp=0.5+(r-0.5)*0.9;
      d=p.x-cp;
      g+=pow(clamp(1.0-abs(d), 0.0, 1.0), 200.0);
      if(d>0.0)
      {
          r=fract(r*4829.013);
          p.x=(p.x-cp)/(1.0-cp);
          v+=1.0;
      }
      else
      {
          r=fract(r*1239.528);
          p.x=p.x/cp;
      }
      p=p.yx;
    }
    
    v/=float(ITER);
    return vec2(g,v);
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

    vec2 st=uv*2.0-1.0;
    st.x*=(_resolution.x/_resolution.y);
    float zfactor=1.0-0.45*length(st);
    //CameraIndex = (fract(_time*0.1)<0.5)? 0 : 1;
    CameraIndex=0;

    vec3 col = vec3(0.0),ro=vec3(0.0,0.0,1.5),ta=vec3(0.0,0.0,0.0);
    if(CameraIndex == 0)
    {
        ln = 50.0*2.0;
        
        ro=vec3(0.0, 0.0, -5.5+mod(0.05*_time+1.0, 3.5));
    }
    else if(CameraIndex == 1)
    {
        ln = 128.0;
        
        ro=vec3(0.0, -5.5+mod(0.05*_time+1.0, 3.5), -5.5+mod(0.05*32.5+1.0, 3.5));
        ta=ro+vec3(0.0,1.0,1.0);
    }
    
    vec3 cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),
    cup=normalize(cross(cdir,cside)),
    rd=normalize(st.x*cside+st.y*cup+zfactor*cdir);
    
    //
    g_ro = ro;
    
    float i=0.0,tvalue=0.0,som=0.0,acc=0.0;mapr mr; // SumOfStep
    for(;++i<ln;){mr=map(ro+rd*(tvalue+=mr.d));if(mr.d<dmin||tvalue>tmax)break;acc+=exp(-3.0*mr.d);}
    som=i/float(ln);
    
    if(mr.hit)
    {
        if(mr.m == 0) // Debug
        {
            col = vec3(exp(-0.25*tvalue));
        }
        else if(mr.m == 1) // Common
        {
            col = vec3(exp(-1.0*tvalue));
        }
        else if(mr.m == 2) // Cyberpunk
        {
            vec3 p=ro+rd*tvalue;
            vec3 n = gn(p);
            if(CameraIndex == 0)
            {
                col = vec3(1.0,0.8,0.6)*max(0.0, dot(normalize(vec3(1.0,-1.0,-1.0)),n));
                col*=vec3(1.0)*max(0.0,1.0-0.5*som*som);
            }
            else
            {
                col=vec3(1.0,0.8,0.6)*15.0/i;
            }
            
            float glow=0.0;
            vec2 gp=DrawPattern(p.xz*0.5);
            glow+=gp.x*max((modc(length(p)-_time*1.5*2.5, 10.0)-9.0)*2.5, 0.0);
            //glow+=gp.x;
            
            if(gp.x<1.3){glow=0.0;}
            // ƒŒƒC‚Æ‚’¼‚Å‚ ‚é‚Ù‚ÇŒõ‚ç‚¹‚é
            glow+=max(1.0-abs(dot(rd,n)) - 0.4, 0.0) * 1.0;
            vec3 glowcol=vec3(1.0,1.0,1.0)*glow*0.25;
            
            // reflect
            if(CameraIndex == 0)
            {
                ro=p;
                rd=reflect(rd,n);
                tvalue=0.1;
                i=0.0;
                mr=mapr(0.0,false,-1);
                for(;++i<ln*0.5;){mr=map(ro+rd*(tvalue+=mr.d));if(mr.d<dmin||tvalue>tmax)break;}
                som=i/float(ln*0.5);
                //col+=vec3(0.0,0.0,1.0)*exp(-1.0*tvalue)*0.1;
                col+=vec3(1.0)*som*0.5;
            }
            
            col.r=pow(col.r,2.2);
            col.g=pow(col.g,2.2);
            col.b=pow(col.b,2.2);
            col=clamp(col,vec3(0.0),vec3(1.0));
            
            col+=glowcol;
        }
        
    }
    
    gl_FragColor = vec4(col,_Alpha);
}

}

