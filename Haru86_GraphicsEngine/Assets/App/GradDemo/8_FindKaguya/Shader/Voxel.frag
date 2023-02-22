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
const int _RenderingTarget = 1;
const float _LeaveStartTime = 1.0;
int _MapIndex;
#else
uniform float _time;
uniform vec2 _resolution;
uniform float _RenderingTarget;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;

uniform int _MapIndex;
uniform float _Alpha;

in vec2 uv;
#endif

// Shared Preprocessor ////////////////////////////////////////////////////////
#define pi 3.14159265
#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define dmin 0.0003
#define tmax 30.0
#define ldir normalize(vec3(1.0,1.0,-1.0))

// Gloabal Valiable
vec3 g_ro;
int CameraIndex;

// Useful Function ////////////////////////////////////////////////////////////

struct mapr // MapResult
{
   float d; // Distance
   bool  hit;
   int   m; // MaterialType
            // 0: Debug
   vec3 vPos;
   vec3 mask;
};

void compm(inout mapr mr,float d,int mt,bool IsMin) // CompareMap
{
    if(IsMin)
    {
        if(d<mr.d) mr = mapr(d,false,mt,vec3(0.0),vec3(0.0));
        if(mr.d<dmin) mr.hit=true; 
    }
    else
    {
        if(d>mr.d) mr = mapr(d,false,mt,vec3(0.0),vec3(0.0));
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

//
vec2 path(in float z){    
   
    //return vec2(0); // Straight.
    float a = sin(z * 0.11);
    float b = cos(z * 0.14);
    return vec2(a*4. -b*1.5, b*1.7 + a*1.5); 
    //return vec2(a*4. -b*1.5, 0.); // Just X.
    //return vec2(0, b*1.7 + a*1.5); // Just Y.
}

//
float rand(vec2 p)
{
    return fract( sin(dot(p,vec2(12.9898,78.233)))*43758.5453123 );
}

// Ray Function ///////////////////////////////////////////////////////////////
float UpDirMap(vec3 p)
{
    p.xy -= path(p.z); // Perturb an object around a path.
   
	p = cos(p*.1575 + sin(p.zxy*.4375)); // 3D sinusoidal mutation.
    
    // Spherize. The result is some mutated, spherical blob-like shapes.
    float n = dot(p, p); 
    
    p = sin(p*3. + cos(p.yzx*3.)); // Finer bumps. Subtle, and almost not necessary with voxelization.
    
    return n - p.x*p.y*p.z*.35 - .9; // Combine, and we're done.
}

float AroundMap(vec3 p)
{
    //p.xy -= path(p.z); // Perturb an object around a path.
   
    p.xz=vec2(atan(p.x,p.z)/3.0*pi,length(p.xz)-1.); // rotate
	p = cos(p*.1575 + sin(p.zxy*.4375)); // 3D sinusoidal mutation.
    
    // Spherize. The result is some mutated, spherical blob-like shapes.
    float n = dot(p, p); 
    
    //p = sin(p*3. + cos(p.yzx*3.)); // Finer bumps. Subtle, and almost not necessary with voxelization.
    
    return n - p.x*p.y*p.z*.35 - .9; // Combine, and we're done.
}

mapr map(vec3 p)
{
    mapr mr;
    mr.d=1000.0;
    mr.hit=false;
    mr.m=-1;
    
    if(_MapIndex == 0){compm(mr, UpDirMap(p) ,0 , true);}
    if(_MapIndex == 1){compm(mr, AroundMap(p) ,0 , true);}
    
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

//
mapr VoxelRayCast(vec3 ro,vec3 rd,const bool IsRef)
{
    //
    vec3 p = floor(ro) + 0.5;
    mapr mr;
    vec3 dRd = 1.0/abs(rd); // ray step ???
    rd = sign(rd);
    vec3 side = dRd*(rd * (p - ro) + 0.5);
    vec3 mask = vec3(0.0);
    
    //
    int num = (IsRef)? 40 : 80;
    for(int i = 0; i< num; i++)
    {
        if( (mr=map(p)).d < 0.0) break;
        
        // maskが重要(?) これがボクセライズを担当？
        mask = step(side, side.yzx) * (1.0 - step(side.zxy, side));
        side += mask*dRd;
        p += mask *rd;
    }
    
    //
    mr.vPos = p;
    mr.mask = mask;
    
    return mr;
}

// Lighting
float voxShadow(vec3 ro, vec3 rd, float end){

    float shade = 1.0;
    vec3 p = floor(ro) + .5;

	vec3 dRd = 1./abs(rd);//1./max(abs(rd), vec3(.0001));
	rd = sign(rd);
    vec3 side = dRd*(rd * (p - ro) + 0.5);
    
    vec3 mask = vec3(0);
    
    float d = 1.;
	
	for (int i = 0; i < 16; i++) {
		
        d = map(p).d;
        
        if (d<0. || length(p-ro)>end) break;
        
        mask = step(side, side.yzx)*(1.-step(side.zxy, side));
		side += mask*dRd;
		p += mask * rd;                
	}

    // Shadow value. If in shadow, return a dark value.
    return shade = step(0., d)*.7 + .3;
    
}

vec4 voxelAO(vec3 p, vec3 d1, vec3 d2) {
   
    // Take the four side and corner readings... at the correct positions...
    // That's the annoying bit that I'm glad others have worked out. :)
	vec4 side = vec4(map(p + d1).d, map(p + d2).d, map(p - d1).d, map(p - d2).d);
	vec4 corner = vec4(map(p + d1 + d2).d, map(p - d1 + d2).d, map(p - d1 - d2).d, map(p + d1 - d2).d);
	
    // Quantize them. It's either occluded, or it's not, so to speak.
    side = step(side, vec4(0));
    corner = step(corner, vec4(0));
    
    // Use the side and corner values to produce a more honed in value... kind of.
    return 1. - (side + side.yzwx + max(corner, side*side.yzwx))/3.;    
	
}

float calcVoxAO(vec3 vp, vec3 sp, vec3 rd, vec3 mask) {
    
    // Obtain four AO values at the appropriate quantized positions.
	vec4 vAO = voxelAO(vp - sign(rd)*mask, mask.zxy, mask.yzx);
    
    // Use the fractional voxel postion and and the proximate AO values
    // to return the interpolated AO value for the surface position.
    sp = fract(sp);
    vec2 uv = sp.yz*mask.x + sp.zx*mask.y + sp.xy*mask.z;
    return mix(mix(vAO.z, vAO.w, uv.x), mix(vAO.y, vAO.x, uv.x), uv.y);

}

// variant of Vorocracks: https://shadertoy.com/view/lsVyRy
// integrated with cracks here: https://www.shadertoy.com/view/Xd3fRN

#define MM 0

#define VARIANT 1              // 1: amplifies Voronoi cell jittering
#if VARIANT
      float ofs = .5;          // jitter Voronoi centers in -ofs ... 1.+ofs
#else
      float ofs = 0.;
#endif
    
//int FAULT = 1;                 // 0: crest 1: fault

float RATIO = 1.,              // stone length/width ratio
 /*   STONE_slope = .3,        // 0.  .3  .3  -.3
      STONE_height = 1.,       // 1.  1.  .6   .7
      profile = 1.,            // z = height + slope * dist ^ prof
 */   
      CRACK_depth = 3.,
      CRACK_zebra_scale = 1.,  // fractal shape of the fault zebra
      CRACK_zebra_amp = .67,
      CRACK_profile = 1.,      // fault vertical shape  1.  .2 
      CRACK_slope = 50.,       //                      10.  1.4
      CRACK_width = .0;    

// === Voronoi =====================================================
// --- Base Voronoi. inspired by https://www.shadertoy.com/view/MslGD8

#define hash22(p)  fract( 18.5453 * sin( p * mat2(127.1,311.7,269.5,183.3)) )
#define disp(p) ( -ofs + (1.+2.*ofs) * hash22(p) )

// --- Voronoi distance to borders. inspired by https://www.shadertoy.com/view/ldl3W8
vec3 voronoiB( vec2 u )  // returns len + id
{
    vec2 iu = floor(u), C, P;
	float m = 1e9,d;
#if VARIANT
    for( int k=0; k < 25; k++ ) {
        vec2  p = iu + vec2(k%5-2,k/5-2),
#else
    for( int k=0; k < 9; k++ ) {
        vec2  p = iu + vec2(k%3-1,k/3-1),
#endif
              o = disp(p),
      	      r = p - u + o;
		d = dot(r,r);
        if( d < m ) m = d, C = p-iu, P = r;
    }

    m = 1e9;
    
    for( int k=0; k < 25; k++ ) {
        vec2 p = iu+C + vec2(k%5-2,k/5-2),
		     o = disp(p),
             r = p-u + o;

        if( dot(P-r,P-r)>1e-5 )
        m = min( m, .5*dot( (P+r), normalize(r-P) ) );
    }

    return vec3( m, P+u );
}

// === pseudo Perlin noise =============================================
#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
int MOD = 1;  // type of Perlin noise
    
// --- 2D
#define hash21(p) fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453123)
float noise2(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p); f = f*f*(3.-2.*f); // smoothstep

    float v= mix( mix(hash21(i+vec2(0,0)),hash21(i+vec2(1,0)),f.x),
                  mix(hash21(i+vec2(0,1)),hash21(i+vec2(1,1)),f.x), f.y);
	return   MOD==0 ? v
	       : MOD==1 ? 2.*v-1.
           : MOD==2 ? abs(2.*v-1.)
                    : 1.-abs(2.*v-1.);
}

#define noise22(p) vec2(noise2(p),noise2(p+17.7))
vec2 fbm22(vec2 p) {
    vec2 v = vec2(0);
    float a = .5;
    mat2 R = rot(.37);

    for (int i = 0; i < 6; i++, p*=2.,a/=2.) 
        p *= R,
        v += a * noise22(p);

    return v;
}

vec3 DrawMarble(vec2 st)
{
    vec3 H0;
    vec3 col = vec3(0.0);
  
    for(float i=0.; i<CRACK_depth ; i++) {
        vec2 V =  st / vec2(RATIO,1),                  // voronoi cell shape
             D = CRACK_zebra_amp * fbm22(st/CRACK_zebra_scale) * CRACK_zebra_scale;
        vec3  H = voronoiB( V + D ); if (i==0.) H0=H;
        
        float d = H.x;                                // distance to cracks
        d = min( 1., CRACK_slope * pow(max(0.,d-CRACK_width),CRACK_profile) );
  
        col += vec3(1.-d) / exp2(i);
        st *= 1.5 * rot(.37);
    }
    
    return col;
}

////////////////////////////////////////////

//
float thickness( in vec3 p, in vec3 n, float maxDist, float falloff )
{
	const float nbIte = 6.0;
	float ao = 0.0;
    
    for( float i=1.; i< nbIte+.5; i++ ){
        
        float l = (i*.75 + fract(cos(i)*45758.5453)*.25)/nbIte*maxDist;
        
        ao += (l + map( p -n*l ).d) / pow(1. + l, falloff);
    }
	
    return clamp( 1.-ao/nbIte, 0., 1.);
}

vec3 dColor(vec3 ro, vec3 rd, float t, mapr mr, vec3 lightPos,const bool IsRef)
{
    vec3 col = vec3(0.0);
    
    // Lighting
    if(mr.hit)
    {
        vec3 p = ro + rd*t;
        vec3 n = -(mr.mask * sign(rd));
    
        if(mr.m == 0) // Debug
        {
            // param
            vec3 p2l = lightPos-p; // pos to ldir
            float lDist = max(length(p2l),0.001);
            p2l /= lDist;
            float atten = 1.0/(1.0 + lDist*0.2 + lDist*0.1);
            
            // shadowing
            float shade = voxShadow(p +n*0.01,p2l,lDist);
            
            // Ambient Occulusion
            float ao = calcVoxAO(mr.vPos, p, rd, mr.mask);
            
            // Diffuse
            float diff = max(0.0, dot(n, p2l));
            // Specular
            float spec = pow(max(0.0, dot( reflect(-p2l,n), -rd )), 32.0);
            //
            float ambient = 0.25;
            
            // Translucency, courtesy of XT95. See the "thickness" function.
            vec3 hf =  normalize(p2l + n);
            float th = thickness( p, n, 1., 1. );
            float tdiff =  pow( clamp( dot(rd, -hf), 0., 1.), 1.);
            float trans = (tdiff + .0)*th;
            
            // marble
            vec2 st_m;
            vec3 n_abs = abs(n);
            if(n_abs.x > n_abs.y && n_abs.x > n_abs.z)
            {
                st_m = p.yz;
            }
            else if(n_abs.y > n_abs.x && n_abs.y > n_abs.z)
            {
                st_m = p.xz;
            }
            else if(n_abs.z > n_abs.y && n_abs.z > n_abs.x)
            {
                st_m = p.xy;
            }
            
            
            // Lighting Result
            vec3 BaseColor = vec3(0.1);
            if(_MapIndex == 1) BaseColor = vec3(0.1,0.0,0.0);

            col = BaseColor * (diff + ambient) + vec3(0.7,0.9,1.0)*spec;
            if(!IsRef)
            {
                vec3 transCol = vec3(0.2); 
                if(_MapIndex == 1) transCol = vec3(0.2,0.0,0.0); 

                col += transCol*trans;
                col += DrawMarble(st_m);
                col *= atten*ao*shade;
            }
             
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
    vec2 st=(gl_FragCoord.xy*2.-_resolution.xy)/min(_resolution.x,_resolution.y);
    _MapIndex = int(floor(mod(_time, 2.0)));
    _MapIndex = 1;
#else
    vec2 st=uv*2.0-1.0;
    st.x*=(_resolution.x/_resolution.y);
#endif
    // Cemra
    float blurPower = 0.1;
    float l_time = _time + (rand(st)*blurPower-blurPower);
    vec3 ro=vec3(0.0), ta=vec3(0.0);
    
    if(_MapIndex == 0)
    {
        ro=vec3(0.0,0.0,1.5 + l_time*20.0), ta = ro + vec3(0.0,0.0,1.0);
        ta.xy += path(ta.z);ro.xy += path(ro.z);
    }
    else if(_MapIndex == 1) 
    {
        ro=vec3(cos(_time*0.1)*5.0,-1.0,sin(_time*0.1)*5.0), ta = vec3(0.0,-2.0,0.0);
    }
    
    vec3 lightPos = ro + vec3(0.0,1.0,5.0);
    if(_MapIndex == 0) lightPos.xy += path(lightPos.z);
    
    float zfactor=1.0-0.45*length(st), t=0.0;
    //zfactor=1.0;
    
    //
    vec3 col = vec3(0.0),cdir=normalize(ta-ro),cside=normalize(cross(vec3(0.0,1.0,0.0),cdir)),
    cup=normalize(cross(cdir,cside)),rd=normalize(st.x*cside+st.y*cup+zfactor*cdir);
    
    //
    vec3 n;vec3 p;bool IsHit;
    {
        // RayTrace
        mapr mr = VoxelRayCast(ro, rd, false);
        vec3 tCube = (mr.vPos - ro - 0.5*sign(rd))/rd;
        t = max(max(tCube.x, tCube.y), tCube.z);
        
        // Lighting
        col = dColor(ro, rd, t, mr, lightPos, false);
        
        //
        p = ro + rd*t;
        n = -(mr.mask * sign(rd));
        IsHit = mr.hit;
    }
    
    // reflection
    if(IsHit)
    {
        //
        ro = p;
        rd = clamp(reflect(rd,n), vec3(-0.5), vec3(0.5));
    
        // RayTrace
        mapr mr = VoxelRayCast(ro, rd, true);
        vec3 tCube = (mr.vPos - ro - 0.5*sign(rd))/rd;
        float ref_t = max(max(tCube.x, tCube.y), tCube.z);
        ref_t = clamp(ref_t, 0.0, 1.0);
        
        // Lighting
        col += dColor(ro, rd, ref_t, mr, lightPos, true);
    }
    
    //
    vec3 fog = mix(vec3(0.96), vec3(0.24), -rd.y*0.5 + 0.5);
    //vec3 fog = vec3(1.0);
    if(_MapIndex == 0)
    {
        col = mix(col, fog*sqrt(fog)*1.2, smoothstep(0.0, 0.95, t/60.0));
    }
    else if(_MapIndex == 1)
    {
        col = mix(col, fog*sqrt(fog)*1.2, smoothstep(0.0, 0.95, t/120.0));
    }
    
    //
    gl_FragColor = vec4(col,_Alpha);
}

}

)"