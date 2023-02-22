R"(

#version 330
// ShaderToy --> OpenGL/GLSL Convert Preprocessor /////////////////////////////
//#define DRAW_ON_SHADERTOY

#ifdef DRAW_ON_SHADERTOY
#define _resolution iResolution
#define gl_FragCoord fragCoord
#define gl_FragColor fragColor
#define _time iTime
#define main() mainImage( out vec4 fragColor, in vec2 fragCoord )
#define _BufferA iChannel0
#else
uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;

uniform sampler2D _BufferA;

in vec2 uv;
#endif

// Shared Preprocessor ////////////////////////////////////////////////////////
#define pi 3.14159265
#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define dmin 0.001
#define tmax 1000.0
#define ldir normalize(vec3(1.0,1.0,-1.0))

// Useful Function ////////////////////////////////////////////////////////////

struct mapr // MapResult
{
   float d; // Distance
   bool  hit;
   int   m; // MaterialType
   float i;
   float t;
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

vec3 trs(vec3 p,vec3 s,vec3 r,vec3 t){p+=t;p.yz*=rot(s.x);p.xz*=rot(s.y);p.xy*=rot(s.z);p*=s;return p;}

vec2 path(in float z){    
    float a = sin(z * 0.11), b = cos(z * 0.14);
    return vec2(a*4. -b*1.5, b*1.7 + a*1.5); 
}

//
float rand(vec2 st){
    return fract(
        sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123
    );
}

// Distance Func
float cube(vec3 p, vec3 s){return length(max(vec3(0.0), abs(p)-s));}

// Map Func
mapr map(vec3 p)
{
    mapr mr = mapr(1000.0, false, -1,0.0,0.0,0.0);
    
    for(float i=0.0; i<20.0; i++)
    {
        float r = 0.25 + rand(vec2(99.99,4545.21) * i) *1.25 ;
        float offsetA = rand(vec2(6.621,1.11148) * i) * 2.0 * 3.1415;
        float offsetB = rand(vec2(1.9987,0.2259) * i) * 2.0 * 3.1415;
        vec3 dir = vec3(
            r*cos(_time+offsetA)*cos(_time+offsetB),
            r*cos(_time+offsetA)*sin(_time+offsetB),
            r*sin(_time+offsetA)
        );
        compm(mr, length(p+dir)-0.001, 0, true);
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

// Ray Func
mapr ray(vec3 ro,vec3 rd, float ln)
{
    mapr mr; float i=0.0,t=0.0,acc=0.0;
    for(;++i<ln;){mr=map(ro+rd*(t+=mr.d));if(mr.d<dmin||mr.t>tmax)break;acc+=exp(-15.0*mr.d);}
    mr.i=i;mr.t=t;mr.acc=acc;
    return mr;
}

vec3 dColor(mapr mr, vec3 ro, vec3 rd, vec2 st)
{
    vec3 p = ro + rd * mr.t; 
    vec3 col = vec3(0.125, 0.291, 0.723)*mr.acc * 2.0;
    return col;
}

// Main ///////
void main()
{
    #ifdef DRAW_ON_SHADERTOY
    vec2 st=(gl_FragCoord.xy*2.-_resolution.xy)/min(_resolution.x,_resolution.y);
    vec2 uv = gl_FragCoord.xy/_resolution.xy;
#else
    vec2 st=uv*2.0-1.0;st.x*=(_resolution.x/_resolution.y);
#endif
    vec3 ro= vec3(1.0*cos(_time),0.0,1.0*sin(_time)),ta=vec3(0.0,0.0,0.0);
    vec3 cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),cup=normalize(cross(cdir,cside)),
    rd=normalize(st.x*cside+st.y*cup+1.0*cdir),col = vec3(0.0);
    
    mapr mr = ray(ro, rd, 64.0);
    col = dColor(mr, ro, rd, st);
    col = mix(col, texture(_BufferA, uv).rgb, 0.981);
    gl_FragColor = vec4(col,1.0);
}

)"