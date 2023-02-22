

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
#define dmin 0.001
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
   vec3 vPos;
   vec3 mask;
   float i;
   float tvalue;
   float acc;
   vec3 vCol;
   float flash;
};

void compm(inout mapr mr,float d,int mt,bool IsMin) // CompareMap
{
    if(IsMin)
    {
        if(d<mr.d) mr = mapr(d,false,mt,vec3(0.0),vec3(0.0),0.0,0.0,0.0,vec3(0.0),0.0);
        if(mr.d<dmin) mr.hit=true; 
    }
    else
    {
        mr.d = max(mr.d, -d);
        /*if(d>mr.d) mr = mapr(d,false,mt,vec3(0.0),vec3(0.0),0.0,0.0,0.0,vec3(0.0),0.0);
        if(mr.d<dmin) mr.hit=true; */
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
vec2 pmod(vec2 p,float n){
  float np = 2.*pi/n;
  float r = atan(p.x,p.y)-0.5*np;
  r = mod(r,np)-0.5*np;
  return length(p.xy)*vec2(cos(r),sin(r));
}
// Distance Function
float cube(vec3 p,vec3 s){
    vec3 q = abs(p);
    vec3 m = max(s-q,0.);
    return length(max(q-s,0.))-min(min(m.x,m.y),m.z);
}
vec4 tetcol(vec3 p,vec3 offset,float scale,vec3 col){
    vec4 z = vec4(p,1.);
    for(int i = 0;i<12;i++){
        if(z.x+z.y<0.0)z.xy = -z.yx,col.z+=1.;
        if(z.x+z.z<0.0)z.xz = -z.zx,col.y+=1.;
        if(z.z+z.y<0.0)z.zy = -z.yz,col.x+=1.;       
        z *= scale;
        z.xyz += offset*(1.0-scale);
    }
    return vec4(col,(cube(z.xyz,vec3(1.5)))/z.w);
}

float Cube(vec3 p,float s)
{
    p=abs(p);
    return length(max(p-vec3(s),0.0));
}

float Triangle(vec3 p,vec3 offset,float scale)
{
    vec4 z=vec4(p.xyz,1.0);
    int count=10;
    
    for(int i=0;i<count;i++)
    {
        if(z.x+z.y<0.0)z.xy=-z.yx;
        if(z.x+z.z<0.0)z.xz=-z.zx;
        if(z.y+z.z<0.0)z.yz=-z.yz;
        z*=scale;
        z.xyz-=offset*(scale-1.0);
    }
    return Cube(z.xyz,1.0)/z.w;
    //return (length(z.xyz)-1.5)/z.w;
}

// Ray Function ///////////////////////////////////////////////////////////////
mapr map(vec3 p)
{
    mapr mr;
    mr.d=1000.0;
    mr.hit=false;
    mr.m=-1;
    
    {
        vec3 lp = p;
        
        lp.xz=pmod(lp.xz,32.0);
        lp.x-=2.1;
        
        lp=abs(lp)-0.5;
        
        lp.yz*=rot(0.5);
        lp.xy*=rot(0.15);
        lp.xz*=rot(0.5);
        
        lp.yz*=rot(pi/2.0);
        
        lp=abs(lp)-0.25;
        lp.z=abs(lp.z)-0.5;
        lp.x=abs(lp.x)-0.75;
        lp.y=abs(lp.y)-0.75;
        
        lp.x=abs(lp.x)-0.65;
        
        lp.x = dot(lp,lp);
        
        float k = 1.4;
        lp=mod(lp,k)-k*0.5;
        
        compm(mr,Triangle(lp,vec3(1.24),1.8), 0, true);
       
        {compm(mr,abs(p.y)-0.5,0,false);}
        
        vec4 sd = tetcol(lp,vec3(1),1.8,vec3(0.));
        float d= sd.w;
        vec3 col = 1.-0.1*sd.xyz-0.3;
        col *= exp(-2.5*d)*2.;
        mr.vCol += col;
    }
    
    
    /*{
         vec3 lp = p;
        // lp.yz*=rot(pi/2.0);
        lp.xz=pmod(lp.xz,32.0);
        lp.x-=2.1;
        
        lp=abs(lp)-0.5;
        
        lp.yz*=rot(0.5);
        lp.xy*=rot(0.15);
        lp.xz*=rot(0.5);
        
        lp.xy*=rot(pi/2.0);
        
        lp=abs(lp)-0.25;
        lp.z=abs(lp.z)-0.25;
        lp.x=abs(lp.x)-0.75;
        lp.y=abs(lp.y)-0.25;
        
        lp.x=abs(lp.x)-1.65;
        
        compm(mr,Triangle(lp,vec3(1.24),1.8), 0, true);
       
        {compm(mr,abs(p.y)-0.5,0,false);}
    }*/
    
    {
        
        vec3 lp = trs(p, vec3(1.5), vec3(0.0) , vec3(0.0));
        compm(mr,Triangle(lp,vec3(1.24),1.8), 1, true);
        
        vec4 sd = tetcol(lp,vec3(1),1.8,vec3(0.));
        float d= sd.w;
        vec3 col = 1.-0.1*sd.xyz-0.3;
        col *= exp(-2.5*d)*2.;
        mr.vCol += col;
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

// RayCast
mapr Raycast(vec3 ro, vec3 rd)
{
    mapr mr;float i=0.0,tvalue=0.0,acc=0.0,flash=0.0;mr.vCol=vec3(0.0);mr.flash=0.0;
    for(;++i<64.0;){
        mr=map(ro+rd*( tvalue+=mr.d*0.75 ));if(mr.d<dmin||tvalue>tmax)break;acc+=exp(-3.0*mr.d);
        if(mod(distance(vec3(0.0),ro+rd*tvalue)-_time*10.0,10.0)<1.0){flash+=exp(-3.0*mr.d);}
    }
    
    mr.i=i; mr.tvalue=tvalue;mr.acc=acc;mr.flash=flash;
 
    return mr;
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
    vec3 ro=vec3(cos(_time)*3.0,0.0,sin(_time)*3.0),ta=vec3(0.0,0.0,0.0);
    vec3 col = vec3(0.0),cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),
    cup=normalize(cross(cdir,cside)),rd=normalize(st.x*cside+st.y*cup+1.0*cdir);ln = 64.0;
    mapr mr = Raycast(ro,rd);
    //if(mr.hit)
    {
        if(mr.m == 0) // Debug
        {
            //col = vec3(1.0)*20.0/mr.i;

            vec3 p = ro + rd * mr.tvalue;
            vec3 n = gn(p);
            float ao = calcAo(p, n);
            float diff = max(0.0, dot(ldir,n));
            col = vec3(0.615, 0.8, 0.88) *10.0/mr.i * mr.acc*0.1 + mr.vCol * mr.acc*0.01;
            col += vec3(1.0)*mr.flash * 0.015;
            
        }
        else if(mr.m == 1)
        {
            //col = vec3(0.615, 0.8, 0.88)*20.0/mr.i;
            
            vec3 p = ro + rd * mr.tvalue;
            vec3 n = gn(p);
            float ao = calcAo(p, n);
            float diff = max(0.0, dot(ldir,n));
            col = vec3(0.615, 0.8, 0.88) *10.0/mr.i + mr.vCol * mr.acc*0.05;
            col += vec3(1.0)*mr.flash * 0.015;
        }
    }
    
     //vec3 fog = mix(vec3(0.96), vec3(0.24), -rd.y*0.5 + 0.5);
     //col = mix(col, fog*sqrt(fog)*1.2, smoothstep(0.0, 0.95, mr.tvalue/60.0));
    
    gl_FragColor = vec4(col,_Alpha);
}

}

