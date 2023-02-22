R"(

#version 330
// ShaderToy --> OpenGL/GLSL Convert Preprocessor /////////////////////////////
//#define DRAW_ON_SHADERTOY

#ifdef DRAW_ON_SHADERTOY
#define _resolution iResolution
#define gl_FragCoord fragCoord
#define gl_FragColor fragColor
#define _time iTime
#define _BufferA iChannel0
#define main() mainImage( out vec4 fragColor, in vec2 fragCoord )
const int _RenderingTarget = 1;
const float _LeaveStartTime = 1.0;
const float _Alpha = 1.0;
#else
uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;

uniform vec3 _AuraPos;
uniform sampler2D _BufferA;
uniform float _Alpha;

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
   float acc;
   float neard;
};

void compm(inout mapr mr,float d,int mt,bool IsMin) // CompareMap
{
    if(IsMin)
    {
        if(d<mr.d) mr = mapr(d,false,mt,0.0,0.0,0.0,0.0);
        if(mr.d<dmin) mr.hit=true; 
    }
    else
    {
        if(d>mr.d) mr = mapr(d,false,mt,0.0,0.0,0.0,0.0);
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

// Camera Path
vec3 path(vec3 scale,vec3 offset)
{
    vec3 r = vec3(0.0,0.0,_time) + offset;
    
    return r;
}

// Noise Function /////////////////////////////////////////////////////////////
float rand(in vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Distance Function
#define STEPS 50
#define EPSILON 0.001
//#define FLAT_SHADING
#define BORDER_LINES
#define BORDER_THICKNESS 1.0
//#define FLAT_GROUND
#define TRIANGLE_BG

// iq's sSqdSegment and sdTriangle functions from: https://www.shadertoy.com/view/XsXSz4
// squared distance to a segment (and orientation)
vec2 sSqdSegment( in vec2 a, in vec2 b, in vec2 p )
{
	vec2 pa = p - a;
	vec2 ba = b - a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return vec2( dot( pa-ba*h, pa-ba*h ), pa.x*ba.y-pa.y*ba.x );
}

// signed distance to a 2D triangle
float sdTriangle( in vec2 v1, in vec2 v2, in vec2 v3, in vec2 p )
{
	vec2 d = min( min( sSqdSegment( v1, v2, p ), 
					   sSqdSegment( v2, v3, p )), 
				       sSqdSegment( v3, v1, p ));

	return -sqrt(d.x)*sign(d.y);
}

// background
vec3 shadeBG( vec2 uv, float sp )
{
	float sp2 = 0.0;
	// TRIANGLE BG
#ifdef TRIANGLE_BG
	vec2 v2 = vec2( -0.12, 0.1 );
	vec2 v1 = vec2( 0.12, 0.1 );
	vec2 v3 = vec2( 0.0,  0.4 );
	float vadd1 = sp * 0.1;
	float vadd2 = sp2;
	float triDist1 = min( 1.0, ( 300.0 * 
						sdTriangle( v1 + vec2( vadd1, -vadd1 ), v2 + vec2( -vadd1, -vadd1 ), v3 + vec2( 0.0, vadd1 ), uv ) ) );
	if ( triDist1 < 0.0 ) // todo: optimize branch
	{
		triDist1 = pow( smoothstep( 0.0, 1.0, abs( triDist1 * 0.05 ) ), 0.1 );
	}
	triDist1 = max( 0.6, triDist1 );
	return vec3( triDist1 ) * 
		vec3( 1.0, 0.95, 0.975 );
#else
	// SPHERE BG
	uv.x *= iResolution.x / iResolution.y;
	uv.y -= 0.25;
	
	vec3 color = vec3( 1.0, 1.0, 1.0 );
	
	float l1 = pow( max( 0.0, min( 1.0, length( uv ) * 4.0 * ( 1.0 + sp  ) ) ), 10.0 );
	float l2 = pow( max( 0.0, min( 1.0, length( uv ) * 6.0 * ( 1.0 + sp2 ) ) ), 10.0 );
	return color * ( max( 1.0 - l1, l2 ) );
#endif
}
int triIsect( const vec3   V1,  // Triangle vertices
			  const vec3   V2,
			  const vec3   V3,
			  const vec3    O,  //Ray origin
			  const vec3    D,  //Ray direction
			  out float res )
{
  vec3 e1, e2;  //Edge1, Edge2
  vec3 P, Q, T;
  float det, inv_det, u, v;
  float t;
 
  e1 = V2 - V1;
  e2 = V3 - V1;

  P = cross( D, e2 );
  det = dot( e1, P );
  if(det > -EPSILON && det < EPSILON) return 0;
  inv_det = 1.0 / det;
 
  T = O - V1;
 
  u = dot(T, P) * inv_det;
  if(u < 0. || u > 1.) return 0;
 
  Q = cross( T, e1 );
 
  v = dot(D, Q) * inv_det;
  if(v < 0. || u + v  > 1.) return 0;
 
  t = dot(e2, Q) * inv_det;
 
  if(t > EPSILON) { //ray intersection
    res = t;
    return 1;
  }
 
  return 0;
}
int triIsectNC( const vec3   V1,  // Triangle vertices
			 const vec3   V2,
			 const vec3   V3,
			 const vec3    O,  //Ray origin
			 const vec3    D,  //Ray direction
			 out float res )
{
  vec3 e1, e2;  //Edge1, Edge2
  vec3 P, Q, T;
  float det, inv_det, u, v;
  float t;
 
  e1 = V2 - V1;
  e2 = V3 - V1;

  P = cross( D, e2 );
  det = dot( e1, P );
  if(det > -EPSILON && det < EPSILON) return 0;
  inv_det = 1.0 / det;
 
  T = O - V1;
 
  u = dot(T, P) * inv_det;
 
  Q = cross( T, e1 );
 
  v = dot(D, Q) * inv_det;
 
  t = dot(e2, Q) * inv_det;
 
  if(t > EPSILON) { //ray intersection
    res = t;
    return 1;
  }
 
  return 0;
}

vec3 polygonalGround( vec3 pos, float zshift, float sp )
{
	float gridSize = 1.0;
	pos.z += zshift;
	vec2 uv1 = floor( pos.xz );
	vec2 uv2 = uv1 + vec2( gridSize, gridSize );
	float um = 0.002;
	float tm = 20.0;
	float gtm = 5.0;

#ifdef FLAT_GROUND
	float h1 = 0.0;
	float h2 = 0.0;
	float h3 = 0.0;
	float h4 = 0.0;
#else
    float t = _time;
    t = 1.0;
	float h1 = sin( gtm * t + tm * rand(  um * uv1 ) );
	float h2 = sin( gtm * t + tm * rand(  um * vec2( uv2.x, uv1.y ) ) );
	float h3 = sin( gtm * t + tm * rand(  um * uv2 ) );
	float h4 = sin( gtm * t + tm * rand(  um * vec2( uv1.x, uv2.y ) ) );
#endif
	
	float hm = 0.7 * max( 0.3, min( 1.0, -( uv1.y - 26.0 ) * 0.05 ) );
	vec3 v1 = vec3( uv1.x, h1 * hm, uv1.y );
	vec3 v2 = vec3( uv2.x, h2 * hm, uv1.y );
	vec3 v3 = vec3( uv2.x, h3 * hm, uv2.y );
	vec3 v4 = vec3( uv1.x, h4 * hm, uv2.y );
	float t1, t2, border1, border2;
	vec3 ro = pos + vec3( 0.0, 100.0, 0.0 );
	vec3 rd = vec3( 0.0, -1.0, 0.0 );
	int tri1res = triIsect( v1, v2, v3, ro, rd, t1 );
	int tri2res = triIsectNC( v1, v3, v4, ro, rd, t2 );
						 
	float h = 0.0;
	if ( tri1res == 1 )
	{
		vec3 pt = ro + rd * t1;
		return ( pt );
	}
	vec3 pt = ro + rd * t2;
	return pt;
}

// Ray Function ///////////////////////////////////////////////////////////////
mapr map(vec3 p)
{
    mapr mr;
    mr.d=1000.0;
    mr.hit=false;
    mr.m=-1;
    
    {compm(mr, p.y - (polygonalGround(p,0.0,1.0).y * (0.75)  - 3.0), 1, true);}
    {
        vec3 lp = p + vec3(0.0,2.0,0.0);
       // compm(mr, length(lp)-0.25, 2, true);
    }
    //{compm(mr,Triangle(trs(p,vec3(1.5),vec3(0.0),vec3(0.0)),vec3(1.24),1.8),0,true);}
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

mapr ray(vec3 ro,vec3 rd,const bool IsRef)
{
    mapr mr;
    
    float i=0.0,t=0.0,acc=0.0;
    for(;++i<ln;){mr=map(ro+rd*(t+=mr.d*0.75));if(mr.d<dmin||mr.t>tmax)break;acc+=exp(-3.0*mr.d);}
    
    mr.i=i;mr.t=t;mr.acc=acc;
    
    return mr;
}

mapr rayaura(vec3 ro, vec3 rd)
{
    mapr mr;
    mr.neard=1000.0;
    
    float r = 0.25 + rand(vec2(99.99,4545.21) * 5.0) *1.25, sp=1.0;
    float offsetA = rand(vec2(6.621,1.11148) * 5.0) * 2.0 * 3.1415;
    float offsetB = rand(vec2(1.9987,0.2259) * 5.0) * 2.0 * 3.1415;
    vec3 dir = vec3(
        4.0*r*cos(_time*sp+offsetA)*cos(_time*sp+offsetB),
        4.0*r*cos(_time*sp+offsetA)*sin(_time*sp+offsetB),
        4.0*r*sin(_time*sp+offsetA)
    );
    
    vec3 ap = vec3(0.0, 2.0, 0.0) + dir;
    float i=0.0,t=0.0,acc=0.0;
    for(;++i<64.0;)
    {
        mr=mapr(length( ro+rd*(t+=mr.d*0.75) + ap)-0.25,false,2,0.0,0.0,0.0,0.0);
        if((mr.d) < mr.neard)
        {
            mr.neard = mr.d;
        }
        if(mr.d<0.001){mr.hit=true; break;}
        if(mr.d>1000.0){break;}
        acc+=exp(-3.0*mr.d);
    }
    
    mr.i=i;mr.t=t;mr.acc=acc;
    
    return mr;
}

// Lighting
float intersectZPlane( vec3 ro, vec3 rd, float planeZ )
{
	return ( -ro.z + planeZ ) / rd.z;
}

// ground
vec3 shadeGround( vec3 eye, vec3 pt, vec3 norm, vec3 normReflection, vec3 light, float mult, float sp )
{
#ifdef FLAT_SHADING
	pt.xz = pt.xz - mod( pt.xz, 1.0 ); // flat shading
#endif
	vec3 r = normalize( reflect( light, norm ) );
	vec3 eyeDir = normalize( pt - eye );
	float dotR = dot( r, eyeDir );
	float diffuseColor = max( 0.0, dotR );
	float ambientColor = 0.7;
	vec3 groundColor = vec3( diffuseColor + ambientColor );
	
	vec3 rd = normalize( reflect( -eyeDir, normReflection ) );
	float t = intersectZPlane( pt, rd, 10.0 );
	vec3 bgPos = pt + rd * t;
	
	float mixv = max( 0.0, dotR * 2.0 );
	vec3 bgColor = shadeBG( abs( bgPos.xy ) * vec2( 0.1, -0.2 ) + vec2( 0.0,2.0 ), sp );
	return groundColor * mixv + bgColor * ( 1.0 - mixv );
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

vec3 DrawBG(vec3 ro, vec3 rd, vec2 st)
{
    vec3 col = vec3(0.2,0.2,0.5)*0.1;
    //col += vec3(exp(-3.0*length(st)));
    
    return col;
}

vec3 dColor(mapr mr, vec3 ro,vec3 rd, vec2 st)
{
    vec3 col = vec3(0.0);
    
    if(mr.hit)
    {
        vec3 p = ro + rd*mr.t;
        vec3 n = gn(p);
        vec3 n2 = normalize( n + vec3(0.0,8.0,0.0) );
    
        if(mr.m == 0) // Debug
        {
            vec3 p = ro + rd * mr.t;
            vec3 n = gn(p);
            float ao = calcAo(p, n);
            float diff = max(0.0, dot(n,ldir)), ambient = 0.25;;
            float spec = pow(max(0.0, dot(reflect(-ldir, n), -rd)), 16.0);
           // col = vec3(0.2,0.2,0.5)*0.1*color2*(diff+ambient) + vec3(1.0)*spec;
            col = vec3(0.2,0.2,0.5) *10.0/mr.i * ao * (diff+ambient);
            
        }
        else if(mr.m == 1) 
        {
            vec3 color1 = shadeBG( st, 1.0 );
            vec3 diffuseColor = vec3( 1.0, 0.95, 0.90 );
            float modx = abs( mod( p.x, 1.0 ) );
            float modz = abs( mod( p.z + 0.0, 1.0 ) );
            float power = 60.0;
            float border = pow(     modx, power )				 // x axis border 
                         + pow( 1.0-modx, power )
                         + pow(     modz, power )				 // z axis border
                         + pow( 1.0-modz, power )				 //
                         + pow( 1.0-abs( modx - modz ), power ); // cross border
            border = max( 0.0, min( 1.0, border * 1.0 ) );
        
            vec3 color2 = shadeGround( ro, p, n, n2, ldir, -1.0, 1.0 ) 
                * diffuseColor + vec3(0.917, 0.569, 0.596)* (  border*4.0 ) ;
            
            col = vec3(exp(-0.25*mr.t));
            
            float dist = ( 1.0 - length( st - vec2( 0.0, 0.25 ) ) * .5) * 1.0;
            
            //
            float diff = max(0.0, dot(n,ldir)), ambient = 0.25;;
            float spec = pow(max(0.0, dot(reflect(-ldir, n), -rd)), 16.0);
            col = vec3(0.2,0.2,0.5)*0.1*color2*(diff+ambient) + vec3(1.0)*spec;
            col += vec3(1.0) * mr.acc * 0.01;
            
            col  *= dist;
        }
        else if(mr.m == 2)
        {
            //col = vec3(0.917, 0.569, 0.6) * pow(-2.0);
        }
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
#ifdef DRAW_ON_SHADERTOY
    vec2 uv = gl_FragCoord.xy/_resolution.xy;
    vec2 st=(gl_FragCoord.xy*2.-_resolution.xy)/min(_resolution.x,_resolution.y);
    //vec3 ro= vec3(3.0*cos(_time),-2.,3.0*sin(_time)),ta=vec3(0.0,-3.0,0.0);
    vec3 ro= vec3(0.0,-1.,5.0),ta=vec3(0.0,-3.0,0.0);
#else
    vec2 st=uv*2.0-1.0;st.x*=(_resolution.x/_resolution.y);
    vec3 ro= _WorldCameraPos,ta=_WorldCameraCenter;
#endif
    vec3 cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),cup=normalize(cross(cdir,cside)),
    rd=normalize(st.x*cside+st.y*cup+1.0*cdir),col = DrawBG(ro,rd,st);
    ln = 128.0;mapr mr;
    
    mr = ray(ro, rd, false);
    if(mr.hit)
    {
        col = dColor(mr,ro,rd,st);
    } 
    
    vec3 fog = mix(vec3(0.96), vec3(0.24), -rd.y*0.5+0.5);
    col = mix(col, fog * sqrt(fog) * 1.2, smoothstep(0.0, 0.95, mr.t/15.0));
    
    mapr mra = rayaura(ro,rd);
    vec3 acol =  vec3(0.917, 0.569, 0.596) * mra.acc ;
    col += acol;
    
    col += texture(_BufferA,uv).rgb;
    
    gl_FragColor = vec4(col,_Alpha);
}

}


)"