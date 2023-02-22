/* File generated with Shader Minifier 1.3.1
 * http://www.ctrl-alt-test.fr
 */

// E:\CppDev\GradDemo\Haru86_GraphicsEngine\Assets\App\GradDemo\0_MoonSea\Shader\MoonSea_Frag.h
"#version 330\n"
 "uniform float _time;"
 "uniform vec2 _resolution;"
 "uniform float _RenderingTarget;"
 "uniform vec3 _WorldCameraPos,_WorldCameraCenter;"
 "uniform float _LeaveStartTime;"
 "uniform int _CameraID;"
 "in vec2 uv;struct mapr{float d;int m;};"
 "void f(inout mapr z,float v,int y,bool m)"
 "{"
   "if(m)"
     "{"
       "if(v<z.d)"
         "z=mapr(v,y);"
     "}"
   "else"
     " if(v>z.d)"
       "z=mapr(v,y);"
 "}\n"
 "#define pi 3.14159265\n"
 "#define rot(a)mat2(cos(a),-sin(a),sin(a),cos(a))\n"
 "float f(vec2 v)"
 "{"
   "float m=dot(v,vec2(127.1,311.7));"
   "return fract(sin(m)*43758.5453123);"
 "}"
 "float v(vec2 v)"
 "{"
   "vec2 m=floor(v),y=fract(v);"
   "float z=f(m+vec2(0.,0.)),s=f(m+vec2(1.,0.)),i=f(m+vec2(0.,1.)),d=f(m+vec2(1.,1.));"
   "vec2 n=y*y*(3.-2.*y);"
   "float S=mix(mix(z,s,n.x),mix(i,d,n.x),n.y);"
   "return 2.*S-1.;"
 "}"
 "float f(float v)"
 "{"
   "return fract(sin(v)*753.5453123);"
 "}"
 "float x(vec3 v)"
 "{"
   "vec3 m=floor(v),y=fract(v);"
   "y=y*y*(3.-2.*y);"
   "float z=m.x+m.y*157.+113.*m.z;"
   "return mix(mix(mix(f(z),f(z+1.),y.x),mix(f(z+157.),f(z+158.),y.x),y.y),mix(mix(f(z+113.),f(z+114.),y.x),mix(f(z+270.),f(z+271.),y.x),y.y),y.z);"
 "}"
 "float m(vec2 f)"
 "{"
   "mat2 z=rot(35.6*pi/180.);"
   "float m=0.,y=0.;"
   "for(float i=0.;i<4.;i++)"
     "{"
       "float S=pow(.5,i+1.);"
       "m+=S*v(f);"
       "f=2.*z*f;"
       "y+=S;"
     "}"
   "return m/y;"
 "}"
 "float f(vec3 v,float m,float y)"
 "{"
   "float f=0.,z=0.;"
   "for(float i=0.;i<m;i++)"
     "{"
       "float S=pow(.5,i+1.)*y;"
       "f+=S*x(v);"
       "v=2.*v;"
       "z+=S;"
     "}"
   "return f/z;"
 "}"
 "float f(vec3 v)"
 "{"
   "return v=50.*fract(v*.3183099+vec3(.71,.113,.419)),-1.+2.*fract(v.x*v.y*v.z*(v.x+v.y+v.z));"
 "}"
 "vec4 t(vec3 v)"
 "{"
   "vec3 z=floor(v),m=fract(v),y=m*m*m*(m*(m*6.-15.)+10.),i=30.*m*m*(m*(m-2.)+1.);"
   "float S=f(z+vec3(0,0,0)),s=f(z+vec3(1,0,0)),d=f(z+vec3(0,1,0)),x=f(z+vec3(1,1,0)),r=f(z+vec3(0,0,1)),e=f(z+vec3(1,0,1)),n=f(z+vec3(0,1,1)),t=f(z+vec3(1,1,1)),c=S,a=s-S,l=d-S,D=r-S,R=S-s-d+x,b=S-d-r+n,p=S-s-r+e,o=-S+s+d-x+r-e-n+t;"
   "return vec4(-1.+2.*(c+a*y.x+l*y.y+D*y.z+R*y.x*y.y+b*y.y*y.z+p*y.z*y.x+o*y.x*y.y*y.z),2.*i*vec3(a+R*y.y+p*y.z+o*y.y*y.z,l+b*y.z+R*y.x+o*y.z*y.x,D+p*y.x+b*y.y+o*y.x*y.y));"
 "}"
 "const mat3 z=mat3(0.,.8,.6,-.8,.36,-.48,-.6,-.48,.64),y=mat3(0.,-.8,-.6,.8,.36,-.48,.6,-.48,.64);"
 "vec4 f(vec3 v,int m)"
 "{"
   "float S=1.9,f=.55,i=0.,r=.5;"
   "vec3 x=vec3(0.);"
   "mat3 e=mat3(1.,0.,0.,0.,1.,0.,0.,0.,1.);"
   "for(int n=0;n<m;n++)"
     "{"
       "vec4 p=t(v);"
       "i+=r*p.x;"
       "x+=r*e*p.yzw;"
       "r*=f;"
       "v=S*z*v;"
       "e=S*y*e;"
     "}"
   "i=i*.5+.5;"
   "return vec4(i,x);"
 "}"
 "vec3 m(vec3 y,vec3 v,vec3 m,vec3 S)"
 "{"
   "return y+=S,y.yz*=rot(v.x),y.xz*=rot(v.y),y.xy*=rot(v.z),y*=v,y;"
 "}\n"
 "#define SEA_FREQ 0.16\n"
 "#define SEA_HEIGHT 0.6\n"
 "#define SEA_CHOPPY 4.0\n"
 "#define ITER_GEOMETRY 3\n"
 "#define ITER_FRAGMENT 5\n"
 "#define SEA_SPEED 0.8\n"
 "#define SEA_TIME (1.0+_time*SEA_SPEED)\n"
 "#define NUM_STEPS 8\n"
 "#define EPSILON_NRM (0.1/_resolution.x)\n"
 "#define SEA_BASE vec3(0.1,0.19,0.22)\n"
 "#define SEA_WATER_COLOR vec3(0.8,0.9,0.6)\n"
 "float m(vec2 z,float y)"
 "{"
   "z+=v(z);"
   "vec2 m=1.-abs(sin(z)),i=abs(cos(z));"
   "m=mix(m,i,m);"
   "return pow(1.-pow(m.x*m.y,.65),y);"
 "}"
 "float r(vec3 v)"
 "{"
   "float y=SEA_FREQ,z=SEA_HEIGHT,i=SEA_CHOPPY;"
   "vec2 S=v.xz;"
   "float f,x=0.;"
   "mat2 r=mat2(1.6,1.2,-1.2,1.6);"
   "for(int n=0;n<ITER_GEOMETRY;n++)"
     "f=m((S+SEA_TIME)*y,i),f+=m((S-SEA_TIME)*y,i),x+=f*z,S*=r,y*=2.,z*=.2,i=mix(i,1.,.2);"
   "return v.y-x;"
 "}"
 "float p(vec3 v)"
 "{"
   "float y=SEA_FREQ,z=SEA_HEIGHT,i=SEA_CHOPPY;"
   "vec2 S=v.xz;"
   "float f,x=0.;"
   "mat2 r=mat2(1.6,1.2,-1.2,1.6);"
   "for(int n=0;n<ITER_FRAGMENT;n++)"
     "f=m((S+SEA_TIME)*y,i),f+=m((S-SEA_TIME)*y,i),x+=f*z,S*=r,y*=2.,z*=.2,i=mix(i,1.,.2);"
   "return v.y-x;"
 "}"
 "float p(vec3 v,vec3 y,out vec3 z,inout bool i)"
 "{"
   "float m=0.,x=1e3,d=r(v+y*x);"
   "if(d>0.)"
     "return z=vec3(0.),i=false,x;"
   "float S=r(v+y*m),f=0.;"
   "for(int n=0;n<NUM_STEPS;n++)"
     "{"
       "float e=S/(S-d);"
       "f=mix(m,x,e);"
       "z=v+y*f;"
       "float p=r(z);"
       "if(p<0.)"
         "x=f,d=p;"
       "else"
         " m=f,S=p;"
     "}"
   "i=true;"
   "return f;"
 "}"
 "mapr s(vec3 v)"
 "{"
   "mapr y;"
   "y.d=1e3;"
   "y.m=-1;"
   "{"
     "float S=clamp(_time-_LeaveStartTime,0.,1.);"
     "f(y,length(m(v,vec3(1.),vec3(0.),vec3(0.,-.35-S-3.5,0.)))-.5,0,true);"
   "}"
   "return y;"
 "}\n"
 "#define EPS (2.0/_resolution.x)\n"
 "vec3 p(vec3 v,bool z)"
 "{"
   "vec3 f=vec3(0.),y=mix(vec3(0.,.038,.038),vec3(0.,.04,.15),v.y*.5+.5),S=vec3(smoothstep(.8,.95,m(100.*v.xy/v.z))),i=vec3(0.);"
   "float x=.25;"
   "for(int n=0;n<3;n++)"
     "i+=m(v.xz/v.y-x+vec2(0.,_time*.5)),x*=1.35;"
   "f=y+.25*i*max(0.,v.y);"
   "f+=S*max(0.,v.y)*2.;"
   "if(z)"
     "{"
       "vec2 n=v.xy/v.z+vec2(0.,.25);"
       "float d=length(n);"
       "vec3 r=vec3(m(n*10.))*smoothstep(.37,.35,d);"
       "r+=vec3(1.)*pow(d*2.25,-16.)*(d<.355?0.:.01);"
       "f+=r*vec3(.5,.4,.15)*2.;"
     "}"
   "return f;"
 "}\n"
 "#define dmin 0.001\n"
 "#define ln 64.0\n"
 "#define ldir normalize(vec3(1.0,1.0,-1.0))\n"
 "vec2 m(vec3 v,int y,inout vec3 z)"
 "{"
   "float i=f(v*.5+vec3(-_time*.25,0.,0.),y).x;"
   "i=smoothstep(-.2,.6,i);"
   "float m=v.y-i*.1;"
   "z=vec3(0.,sign(v.y),0.);"
   "return vec2(m,i);"
 "}"
 "vec3 f(vec3 v,vec3 y,vec3 i,vec3 z,vec3 m)"
 "{"
   "float f=clamp(1.-dot(y,-z),0.,1.);"
   "f=pow(f,3.)*.65;"
   "vec3 S=p(reflect(z,y),true),x=SEA_BASE+pow(dot(y,i)*.4+.6,80.)*SEA_WATER_COLOR*.12,e=mix(S,x,f);"
   "float n=max(1.-dot(m,m)*.001,0.);"
   "e+=SEA_WATER_COLOR*(v.y-SEA_HEIGHT)*.18*n;"
   "e+=vec3(pow(max(dot(reflect(z,y),i),0.),60.)*(68./(pi*8.)));"
   "return e;"
 "}"
 "vec4 p(vec3 v,vec3 y,vec4 i)"
 "{"
   "float z=0.,f=0.,x=0.,r=0.;"
   "vec4 n=vec4(0.);"
   "for(;++x<ln;x++)"
     "{"
       "vec3 S=v+y*f,d;"
       "vec2 p=m(S,5,d);"
       "z=p.x;"
       "r=p.y;"
       "if(z>dmin)"
         "{"
           "vec4 e=vec4(vec3(mix(0.,1.,r)),r*1.2);"
           "e.w*=.4;"
           "e.xyz*=e.w;"
           "vec3 s=normalize(d);"
           "float t=clamp(dot(s,ldir),0.,1.);"
           "e.xyz*=t;"
           "n+=e*(1.-n.w)*clamp(1.3-S.y*.2,0.,1.);"
         "}"
       "f+=max(.05,.1*z);"
     "}"
   "n=max(vec4(0.),n);"
   "i.xyz=mix(i.xyz,n.xyz+vec3(.5),n.w);"
   "float S=1.-clamp(f/30.,0.,1.);"
   "vec3 d=mix(vec3(0.,.038,.038),vec3(0.,.04,.15),mod(y.y,1.)*.5+.5);"
   "i.xyz=mix(d,i.xyz,S);"
   "return i;"
 "}\n"
 "#define tile 0.850\n"
 "vec3 r(vec3 v,vec3 y)"
 "{"
   "vec3 z=vec3(0.);"
   "float i=.1,f=1.;"
   "v.x+=1.;"
   "for(int n=0;n<20;n++)"
     "{"
       "vec3 m=v+y*i;"
       "float S=0.,r=0.;"
       "m=abs(vec3(tile)-mod(m,vec3(tile*2.)));"
       "for(int x=0;x<17;x++)"
         "{"
           "float e=2.;"
           "m=abs(m)/dot(m,m)*e-.5*e;"
           "r+=abs(length(m)-S);"
           "S=length(m);"
         "}"
       "r*=r*r;"
       "z+=vec3(i,i*i,i*i*i*i)*r*.0015*f;"
       "f*=.73;"
       "i+=.1;"
     "}"
   "return z*.01;"
 "}"
 "vec3 s(vec3 v,float m)"
 "{"
   "vec3 y;"
   "y.y=p(v);"
   "y.x=p(vec3(v.x+m,v.y,v.z))-y.y;"
   "y.z=p(vec3(v.x,v.y,v.z+m))-y.y;"
   "y.y=m;"
   "return normalize(y);"
 "}"
 "vec3 n(vec3 v)"
 "{"
   "vec2 y=vec2(dmin,0.);"
   "return normalize(vec3(s(v+y.xyy).d-s(v-y.xyy).d,s(v+y.yxy).d-s(v-y.yxy).d,s(v+y.yyx).d-s(v-y.yyx).d));"
 "}"
 "void main()"
 "{"
   "if(_RenderingTarget==2)"
     "gl_FragColor=vec4(vec3(0.),1.);"
   "else"
     "{"
       "vec2 v=uv*2.-1.;"
       "v.x*=_resolution.x/_resolution.y;"
       "float m=max(0.,_time-_LeaveStartTime),i=clamp(_time-_LeaveStartTime,0.,1.);"
       "vec3 y=_WorldCameraPos,z=_WorldCameraCenter;"
       "y.y+=3.5;"
       "z.y+=3.5;"
       "if(i>=1.)"
         "{"
           "y=vec3(0.,m,1.5),z=vec3(0.,m,0.);"
           "if(_CameraID==0)"
             "z+=2.,y.xz*=rot(_time);"
           "else"
             " if(_CameraID==1)"
               "y=y+_WorldCameraPos,z=z+_WorldCameraCenter;"
             "else"
               " if(_CameraID==2)"
                 "y+=vec3(2.);"
         "}"
       "vec4 S=vec4(vec3(0.),1.);"
       "vec3 x=normalize(z-y),n=normalize(cross(vec3(0.,1.,0.),x)),d=normalize(cross(x,n)),e=normalize(v.x*n+v.y*d+x);"
       "float t=0.,c=0.,a=0.;"
       "mapr l;"
       "if(i<1.)"
         "{"
           "for(;++t<ln;)"
             "{"
               "l=s(y+e*(c+=l.d));"
               "if(l.d<dmin)"
                 "break;"
               "a+=exp(-3.*l.d);"
             "}"
           "S.xyz=p(e,false);"
           "S.xyz=smoothstep(0.,1.,S.xyz);"
           "S.xyz*=1.2;"
           "vec3 R=pow(vec3(1.),vec3(2.2));"
           "S.xyz+=R*(l.d<dmin?1.:a)*.02;"
           "if(l.m==0)"
             "{"
               "vec3 u=y+e*c;"
               "u.y-=m;"
               "S.xyz=vec3(1.-f(7.5*u,4.,1.))+vec3(.5,.4,.15);"
             "}"
           "{"
             "vec3 u;"
             "bool D=false;"
             "p(y+vec3(0.,m>0.?(m+=.25):0.,0.),e,u,D);"
             "vec3 E=u-y,b=s(u,dot(E,E)*EPSILON_NRM);"
             "if(D)"
               "{"
                 "S.xyz=vec3(1.)*max(0.,dot(b,ldir));"
                 "vec3 Y=f(u,b,ldir,e,E);"
                 "S.xyz=Y;"
               "}"
           "}"
         "}"
       "vec4 u=vec4(vec3(0.),1.);"
       "if(_time>=_LeaveStartTime)"
         "{"
           "float E=clamp((_time-_LeaveStartTime-1.-5.)/9.,0.,1.);"
           "vec4 D=p(y,e,S),b=vec4(r(y,e),1.);"
           "u=mix(D,b,E);"
         "}"
       "S=mix(S,u,i);"
       "gl_FragColor=S;"
     "}"
 "}",

