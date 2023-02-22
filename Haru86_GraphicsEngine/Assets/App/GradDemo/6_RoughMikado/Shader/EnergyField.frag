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
const int _RenderingTarget = 1;
const float _LeaveStartTime = 1.0;
#else
uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;
uniform sampler2D _BufferA;

in vec2 uv;
#endif

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
   float t;     
   float i;
};

void compm(inout mapr mr,float d,int mt,bool IsMin) // CompareMap
{
    if(IsMin)
    {
        if(d<mr.d) mr = mapr(d,false,mt,0.0,0.0);
        if(mr.d<dmin) mr.hit=true; 
    }
    else
    {
        if(d>mr.d) mr = mapr(d,false,mt,0.0,0.0);
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

// Noise Func //////////
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

// SNOISE function from: https://www.shadertoy.com/view/lsf3RH
float snoise(vec3 uv, float res)
{
	const vec3 s = vec3(1e0, 1e2, 1e3);
	
	uv *= res;
	
	vec3 uv0 = floor(mod(uv, res))*s;
	vec3 uv1 = floor(mod(uv+vec3(1.), res))*s;
	
	vec3 f = fract(uv); f = f*f*(3.0-2.0*f);

	vec4 v = vec4(uv0.x+uv0.y+uv0.z, uv1.x+uv0.y+uv0.z,
		      	  uv0.x+uv1.y+uv0.z, uv1.x+uv1.y+uv0.z);

	vec4 r = fract(sin(v*1e-1)*1e3);
	float r0 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	r = fract(sin((v + uv1.z - uv0.z)*1e-1)*1e3);
	float r1 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	return mix(r0, r1, f.z)*2.-1.;
}

float noise(vec3 p, float baseres)
{
    float n = 0.0;
    for (int i = 0; i < 7; i++)
    {
        float v = pow(2.0, float(i));
        n += (1.5 / v) * snoise(p + vec3(1.) * (float(i) / 17.), v * baseres);
    }
    
    
    return clamp((1.0 - n) * .5, 0., 1.) * 2.0;
}

// Ray Func ////
vec2 CloudMap(vec3 p,int num)
{
    float f = fbm2(p*0.5+vec3(-_time*0.25,0.0,0.0),num).x;
    f=smoothstep(-0.2,0.6,f);
    float d = p.y - f*0.1;
    return vec2(d,f);
}

mapr map(vec3 p)
{
    mapr mr = mapr(1000.0,false,-1,0.0,0.0);
    
    //{compm(mr, length(p)-0.5, 0, true);}
    {
        float h = fbm2(p, 9).x;
        compm(mr, p.y+1.0+h, 1, true);
    }
    //{compm(mr, CloudMap(p,9).x, 0, true);}
    
    return mr;
}

mapr ray(vec3 ro, vec3 rd)
{
    mapr mr;
    float i=0.0,t=0.0,acc=0.0;
    for(;++i<64.0;)
    {
        mr=map(ro+rd*(t+=mr.d*0.75));
       
        if(mr.d<0.001){mr.hit=true; break;}
        if(mr.d>1000.0){break;}
        acc+=exp(-3.0*mr.d);
    }
    
    mr.i=i;mr.t=t;
    
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

vec4 DrawAura(vec2 uv)
{
    float aspectRatio = _resolution.y/_resolution.x;
    
    uv.y *= aspectRatio;
    
     // Tweaking vars
    vec4 color = vec4(0.125, 0.291, 0.723, 1.0);
    vec4 leaving = vec4(0.925, 0.791, 0.323, 1.0);
    float noise_sz = 7.0f;
    float speed = 0.4;
    vec2 center = vec2(0.5, 0.5 * aspectRatio);
    
    float dc = 1. - (distance(uv, center) * 2.);
    float pdc = pow(dc, 3.5);
    
    vec2 dir = -normalize(uv - center) * speed;
    
    float phase0 = fract(_time * 0.3 + 0.5);
    float phase1 = fract(_time * 0.3 + 0.0);
    
    vec2 uv0 = uv + phase0 * dir;
    vec2 uv1 = uv + phase1 * dir;
    
    // Rotation
    float as = pdc * sin(_time * 0.9) * 1.2;
	float ca = cos(as);
	float sa = sin(as);					
    
    mat2 rot;
    rot[0] = vec2(ca, -sa);
    rot[1] = vec2(sa, ca);
    
    uv0 = center + ((uv0 - center) * rot);
    uv1 = center + ((uv1 - center) * rot);

    // Samplings <-- raymarchŒ‹‰Ê‚Ìpos‚ð“n‚¹‚Î‚¢‚¢‚Ì‚Å‚ÍH
    float tex0 = max(noise(vec3(uv0,1.0), noise_sz), noise(vec3(uv0,1.0) * 1.2, noise_sz));
    float tex1 = max(noise(vec3(uv1,1.0), noise_sz), noise(vec3(uv1,1.0) * 1.4, noise_sz));
    
    float lerp = abs((0.5 - phase0) / 0.5);
    float samplings = mix(tex0, tex1, lerp);
    
    // Color
    vec4 c = vec4(samplings, samplings, samplings, 1.0) * mix(color, leaving, pdc) * pdc;
  	c += pow(dc, 16.0) * mix(color, leaving, pow(dc, 16.0)) * 2.3;
    c = max(vec4(0.0), c);
    return c;
}

vec3 dColor(mapr mr, vec3 ro, vec3 rd)
{
    vec3 col = vec3(0.0);
    
    if(mr.hit)
    {
        if(mr.m == 0)
        {
            col = vec3(1.0) * exp(-1.0*mr.t);
        }
        else if(mr.m == 1)
        {
            vec3 p = ro + rd * mr.t;
            vec3 n = gn(p);
            float diff = max(0.0, dot(n, ldir));
            float spec = pow(max(0.0, dot(reflect(-ldir, n), -rd)), 16.0);
            float ambient = 0.1;
            col = vec3(0.1) * (diff + ambient) + vec3(1.0) * spec;
        }
    }
    
    return col;
}

//
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
    ln = 16.0;mapr mr;
    
    mr = ray(ro, rd);
    col = dColor(mr, ro, rd);
    
    vec3 fog = mix(vec3(0.1), vec3(0.85), -rd.y*0.5+0.5) *0.25;
    col = mix(col, fog * sqrt(fog) * 1.2, smoothstep(0.0, 0.95, mr.t/10.0));
    
    col += DrawAura(uv).rgb;
      
    
  
    col += texture(_BufferA,uv).rgb;
  
    gl_FragColor = vec4(col,1.0);
}

)"