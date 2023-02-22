/* File generated with Shader Minifier 1.3.1
 * http://www.ctrl-alt-test.fr
 */

// E:\CppDev\GradDemo\Haru86_GraphicsEngine\Assets\App\GradDemo\0_MoonSea\Shader\RaySpaceShip_Frag.h
"#version 330\n"
 "uniform float _time;"
 "uniform vec2 _resolution;"
 "uniform float _RenderingTarget;"
 "uniform vec3 _WorldCameraPos,_WorldCameraCenter;"
 "uniform int _IsUseShowing;"
 "uniform float _ShowingFinTime,_ShowDuration,_MoveStartTime,_MoveTimeDuration,_MoveH;"
 "uniform int _TRSIndex;"
 "uniform float _GoMoonTime;"
 "uniform int _RefMapIndex;"
 "uniform sampler2D _BufferA;"
 "uniform float _CorrectionValue;"
 "uniform int _UseCustomPos;"
 "uniform vec3 _CustomRo,_CustomTa;"
 "in vec2 uv;\n"
 "#define pi 3.14159265\n"
 "#define rot(a)mat2(cos(a),-sin(a),sin(a),cos(a))\n"
 "#define dmin 1.0\n"
 "#define tmax 3000.0\n"
 "#define ldir normalize(vec3(1.0,1.0,-1.0))\n"
 "vec3 m;"
 "int e;"
 "float i;"
 "vec3 v;"
 "float f(float e,float m,float x)"
 "{"
   "float v=exp(-x*e)+exp(-x*m);"
   "return-log(v)/x;"
 "}struct mapr{float d;bool hit;int m;float tvalue;float i;float acc;vec3 jetCol;float metallic;};"
 "void f(inout mapr v,float m,int e,bool a,bool y)"
 "{"
   "if(a)"
     "if(!y)"
       "{"
         "if(m<v.d)"
           "v=mapr(m,false,e,0.,0.,0.,vec3(0.),0.);"
         "if(v.d<dmin)"
           "v.hit=true;"
       "}"
     "else"
       "{"
         "if(m<v.d)"
           "v=mapr(f(m,v.d,1.5),false,e,0.,0.,0.,vec3(0.),0.);"
         "if(v.d<dmin)"
           "v.hit=true;"
       "}"
   "else"
     "{"
       "if(m>v.d)"
         "v=mapr(m,false,e,0.,0.,0.,vec3(0.),0.);"
       "if(v.d<dmin)"
         "v.hit=true;"
     "}"
 "}"
 "vec3 f(vec3 v,vec3 m,vec3 e,vec3 y)"
 "{"
   "return v+=y,v.yz*=rot(e.x),v.xz*=rot(e.y),v.xy*=rot(e.z),v*=m,v;"
 "}"
 "float f(vec3 v)"
 "{"
   "return v=50.*fract(v*.3183099+vec3(.71,.113,.419)),-1.+2.*fract(v.x*v.y*v.z*(v.x+v.y+v.z));"
 "}"
 "vec4 n(vec3 v)"
 "{"
   "vec3 m=floor(v),a=fract(v),e=a*a*a*(a*(a*6.-15.)+10.),x=30.*a*a*(a*(a-2.)+1.);"
   "float y=f(m+vec3(0,0,0)),l=f(m+vec3(1,0,0)),s=f(m+vec3(0,1,0)),d=f(m+vec3(1,1,0)),t=f(m+vec3(0,0,1)),p=f(m+vec3(1,0,1)),z=f(m+vec3(0,1,1)),c=f(m+vec3(1,1,1)),i=y,r=l-y,b=s-y,n=t-y,h=y-l-s+d,o=y-s-t+z,u=y-l-t+p,C=-y+l+s-d+t-p-z+c;"
   "return vec4(-1.+2.*(i+r*e.x+b*e.y+n*e.z+h*e.x*e.y+o*e.y*e.z+u*e.z*e.x+C*e.x*e.y*e.z),2.*x*vec3(r+h*e.y+u*e.z+C*e.y*e.z,b+o*e.z+h*e.x+C*e.z*e.x,n+u*e.x+o*e.y+C*e.x*e.y));"
 "}"
 "const mat3 x=mat3(0.,.8,.6,-.8,.36,-.48,-.6,-.48,.64),y=mat3(0.,-.8,-.6,.8,.36,-.48,.6,-.48,.64);"
 "vec4 f(vec3 v,int e)"
 "{"
   "float m=1.9,a=.55,f=0.,i=.5;"
   "vec3 r=vec3(0.);"
   "mat3 u=mat3(1.,0.,0.,0.,1.,0.,0.,0.,1.);"
   "for(int t=0;t<e;t++)"
     "{"
       "vec4 s=n(v);"
       "f+=i*s.x;"
       "r+=i*u*s.yzw;"
       "i*=a;"
       "v=m*x*v;"
       "u=m*y*u;"
     "}"
   "f=f*.5+.5;"
   "return vec4(f,r);"
 "}\n"
 "#define tile 0.850\n"
 "vec3 n(vec3 v,vec3 y)"
 "{"
   "vec3 m=vec3(0.);"
   "float e=.1,f=1.;"
   "v.x+=1.;"
   "for(int i=0;i<20;i++)"
     "{"
       "vec3 a=v+y*e;"
       "float r=0.,x=0.;"
       "a=abs(vec3(tile)-mod(a,vec3(tile*2.)));"
       "for(int u=0;u<17;u++)"
         "{"
           "float t=2.;"
           "a=abs(a)/dot(a,a)*t-.5*t;"
           "x+=abs(length(a)-r);"
           "r=length(a);"
         "}"
       "x*=x*x;"
       "m+=vec3(e,e*e,e*e*e*e)*x*.0015*f;"
       "f*=.73;"
       "e+=.1;"
     "}"
   "return m*.01;"
 "}"
 "vec3 n(vec3 m,vec3 v,vec3 e)"
 "{"
   "return n(reflect(-v,e),v);"
 "}"
 "float s(vec2 v)"
 "{"
   "return max(v.y,v.x);"
 "}"
 "float s(vec3 v)"
 "{"
   "return max(v.z,s(v.xy));"
 "}"
 "vec3 s(vec3 e,float v)"
 "{"
   "float m=sin(v),a=cos(v);"
   "return mat3(a,-m,0,m,a,0,0,0,1)*e;"
 "}"
 "vec3 t(vec3 v)"
 "{"
   "return v=s(v,(floor(atan(v.y,v.x)*1.5/acos(-1.))*2.+1.)*acos(-1.)/3.),vec3(v.x,abs(v.y),v.z);"
 "}"
 "float a(vec3 v)"
 "{"
   "vec3 e=abs(v);"
   "return max(e.y,dot(vec3(.87,.5,0),e))-1.;"
 "}"
 "float p(vec3 v)"
 "{"
   "return max(max(abs(length(v-vec3(0,0,5.))-5.)-.05,a(v)),v.z-2.);"
 "}"
 "float r(vec3 v)"
 "{"
   "return max(abs(length(v*vec3(1,1,.3))-.325)-.025,-v.z);"
 "}"
 "float h(vec3 v)"
 "{"
   "vec3 e=abs(v);"
   "return max(s(e),max(max(length(e.xy),length(e.yz)),length(e.zx))-.2)-1.;"
 "}"
 "float d(vec3 v)"
 "{"
   "vec3 m=v*20.,e=t(m*2.+vec3(0,0,2))-vec3(1.4,0,0),f=m;"
   "f.y=abs(f.y);"
   "return min(min(min(max(a(f*4.-vec3(2,5,0))*.25,abs(f.z)-1.),p(f.yzx*vec3(1,.5,.5)*1.5+vec3(.3,0,0))/1.5),max(min(.1-abs(f.x),-f.z),p(vec3(0,0,1)-f.xzy*vec3(1,.5,.5)))),min(min(max(h(e),-h(e*1.2+vec3(0,0,1.5))/1.2),r(e+vec3(0,0,1.5))),r(vec3(abs(e.xy),e.z)-vec3(.5,.5,-1.5)))*.5)*.05;"
 "}"
 "mapr l(vec3 v)"
 "{"
   "mapr e=mapr(1e3,false,-1,0.,0.,0.,vec3(0.),0.);"
   "if(_TRSIndex==0)"
     "{"
       "float m=clamp((_time-_MoveStartTime)/_MoveTimeDuration,0.,1.);"
       "v=mix(f(v,vec3(1.),vec3(0.,0.,pi*.5),vec3(0.)),f(v,vec3(1.),vec3(1.5525,sin(_time)*.15,sin(_time)*.15+pi*.5),vec3(0.,-.1,0.)),m);"
     "}"
   "else"
     " if(_TRSIndex==1)"
       "{"
         "vec3 m=vec3(0.);"
         "if(_time>=_GoMoonTime)"
           "m.xz+=(exp(_time-_GoMoonTime)-exp(.001))*10.;"
         "float i=0.;"
         "i=mix(0.,atan(-1.,1.),clamp(_time-_GoMoonTime,0.,1.));"
         "v=f(v,vec3(1.),vec3(2.925,2.925+pi+i,pi*1.1+sin(_time)*.15+pi*.5),m);"
       "}"
     "else"
       " v=f(v,vec3(1.),vec3(0.,sin(_time)*.15,sin(_time)*.15+pi*.5),vec3(0.));"
   "float m=clamp((_time-_ShowingFinTime+_ShowDuration)/_ShowDuration,0.,1.);"
   "if(m<1.)"
     "{"
       "float x=m<.001?0.:f(v*10.+_time*pow(m,2.),5).x*m;"
       "f(e,mix(length(v)-.12*min(1.,m)-x*.05,d(v),m),0,true,false);"
       "e.metallic=mix(1.,.5,m);"
     "}"
   "else"
     " f(e,d(v),0,true,false),e.metallic=.5;"
   "return e;"
 "}"
 "mapr a(vec3 m,vec3 e,bool y)"
 "{"
   "float a=0.,f=0.,i=0.;"
   "mapr r;"
   "vec3 t=vec3(0.);"
   "for(f=0.;f<80.;++f)"
     "{"
       "vec3 x=m+e*a;"
       "r=l(x);"
       "if(abs(r.d)<(a*5.+1.)*1e-4||a>=3e3)"
         "break;"
       "i+=exp(-3.*r.d);"
       "a=min(a+r.d,3e3);"
       "vec3 d=v-x;"
       "t+=exp(-1.*length(d))*vec3(.33,.33,.88);"
     "}"
   "r.i=f;"
   "r.tvalue=a;"
   "r.acc=i;"
   "r.jetCol=t;"
   "return r;"
 "}"
 "vec3 c(vec3 v)"
 "{"
   "vec2 e=vec2(.001,0.);"
   "return normalize(vec3(l(v+e.xyy).d-l(v-e.xyy).d,l(v+e.yxy).d-l(v-e.yxy).d,l(v+e.yyx).d-l(v-e.yyx).d));"
 "}"
 "void main()"
 "{"
   "vec2 e=uv*2.-1.;"
   "e.x*=_resolution.x/_resolution.y;"
   "vec3 m=_WorldCameraPos,f=_WorldCameraCenter;"
   "if(_MoveH>0.)"
     "m.y-=_MoveH,f.y-=_MoveH;"
   "m*=_CorrectionValue;"
   "if(_UseCustomPos==1)"
     "m=_CustomRo,f=_CustomTa;"
   "vec3 y=normalize(f-m),x=normalize(cross(vec3(0.,1.,0.),y)),i=normalize(cross(y,x)),r=normalize(e.x*x+e.y*i+y),u=vec3(0.);"
   "v=vec3(0.);"
   "mapr t=a(m,r,false);"
   "vec3 d=m+r*t.tvalue;"
   "float s=0.;"
   "if(t.hit&&t.m==0)"
     "{"
       "vec3 l=c(d),z=abs(l);"
       "float h=max(0.,dot(z,ldir)),p=pow(max(0.,dot(reflect(-ldir,z),-r)),16.);"
       "u=t.jetCol*.05*(h+.25)+vec3(1.)*p;"
       "{"
         "m=d;"
         "r=reflect(r,l);"
         "vec3 b=n(m,r,l);"
         "u=mix(u,b,t.metallic);"
       "}"
       "s=1.;"
     "}"
   "gl_FragColor=vec4(u,s);"
 "}",

