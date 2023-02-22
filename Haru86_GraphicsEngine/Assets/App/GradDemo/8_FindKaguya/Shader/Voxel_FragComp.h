/* File generated with Shader Minifier 1.3.1
 * http://www.ctrl-alt-test.fr
 */

// E:\CppDev\GradDemo\Haru86_GraphicsEngine\Assets\App\GradDemo\8_FindKaguya\Shader\Voxel_Frag.h
"#version 330\n"
 "uniform float _time;"
 "uniform vec2 _resolution;"
 "uniform float _RenderingTarget;"
 "uniform vec3 _WorldCameraPos,_WorldCameraCenter;"
 "uniform int _MapIndex;"
 "uniform float _Alpha;"
 "in vec2 uv;"
 "const float x=3.14159265;\n"
 "#define rot(a)mat2(cos(a),-sin(a),sin(a),cos(a))\n"
 "const float z=3e-4,v=30.;"
 "const vec3 f=normalize(vec3(1.,1.,-1.));"
 "vec3 m;"
 "int e;struct mapr{float d;bool hit;int m;vec3 vPos;vec3 mask;};"
 "void t(inout mapr v,float d,int f,bool x)"
 "{"
   "if(x)"
     "{"
       "if(d<v.d)"
         "v=mapr(d,false,f,vec3(0.),vec3(0.));"
       "if(v.d<z)"
         "v.hit=true;"
     "}"
   "else"
     "{"
       "if(d>v.d)"
         "v=mapr(d,false,f,vec3(0.),vec3(0.));"
       "if(v.d<z)"
         "v.hit=true;"
     "}"
 "}"
 "vec3 n(vec3 v,vec3 d,vec3 f,vec3 x)"
 "{"
   "return v+=x,v.yz*=rot(d.x),v.xz*=rot(d.y),v.xy*=rot(d.z),v*=d,v;"
 "}"
 "vec2 n(float v)"
 "{"
   "float x=sin(v*.11),c=cos(v*.14);"
   "return vec2(x*4.-c*1.5,c*1.7+x*1.5);"
 "}"
 "float t(vec2 v)"
 "{"
   "return fract(sin(dot(v,vec2(12.9898,78.233)))*43758.5453123);"
 "}"
 "float s(vec3 v)"
 "{"
   "v.xy-=n(v.z);"
   "v=cos(v*.1575+sin(v.zxy*.4375));"
   "float x=dot(v,v);"
   "v=sin(v*3.+cos(v.yzx*3.));"
   "return x-v.x*v.y*v.z*.35-.9;"
 "}"
 "float r(vec3 v)"
 "{"
   "v.xz=vec2(atan(v.x,v.z)/3.*x,length(v.xz)-1.);"
   "v=cos(v*.1575+sin(v.zxy*.4375));"
   "float f=dot(v,v);"
   "return f-v.x*v.y*v.z*.35-.9;"
 "}"
 "mapr c(vec3 v)"
 "{"
   "mapr f;"
   "f.d=1e3;"
   "f.hit=false;"
   "f.m=-1;"
   "if(_MapIndex==0)"
     "t(f,s(v),0,true);"
   "if(_MapIndex==1)"
     "t(f,r(v),0,true);"
   "return f;"
 "}"
 "vec3 p(vec3 v)"
 "{"
   "vec2 f=vec2(.001,0.);"
   "return normalize(vec3(c(v+f.xyy).d-c(v-f.xyy).d,c(v+f.yxy).d-c(v-f.yxy).d,c(v+f.yyx).d-c(v-f.yyx).d));"
 "}"
 "mapr c(vec3 v,vec3 f,const bool x)"
 "{"
   "vec3 i=floor(v)+.5;"
   "mapr d;"
   "vec3 m=1./abs(f);"
   "f=sign(f);"
   "vec3 y=m*(f*(i-v)+.5),r=vec3(0.);"
   "int z=x?40:80;"
   "for(int u=0;u<z;u++)"
     "{"
       "if((d=c(i)).d<0.)"
         "break;"
       "r=step(y,y.yzx)*(1.-step(y.zxy,y));"
       "y+=r*m;"
       "i+=r*f;"
     "}"
   "d.vPos=i;"
   "d.mask=r;"
   "return d;"
 "}"
 "float n(vec3 v,vec3 f,float x)"
 "{"
   "vec3 i=floor(v)+.5,m=1./abs(f);"
   "f=sign(f);"
   "vec3 d=m*(f*(i-v)+.5),r=vec3(0);"
   "float y=1.;"
   "for(int u=0;u<16;u++)"
     "{"
       "y=c(i).d;"
       "if(y<0.||length(i-v)>x)"
         "break;"
       "r=step(d,d.yzx)*(1.-step(d.zxy,d));"
       "d+=r*m;"
       "i+=r*f;"
     "}"
   "return step(0.,y)*.7+.3;"
 "}"
 "vec4 p(vec3 v,vec3 f,vec3 x)"
 "{"
   "vec4 d=vec4(c(v+f).d,c(v+x).d,c(v-f).d,c(v-x).d),i=vec4(c(v+f+x).d,c(v-f+x).d,c(v-f-x).d,c(v+f-x).d);"
   "d=step(d,vec4(0));"
   "i=step(i,vec4(0));"
   "return 1.-(d+d.yzwx+max(i,d*d.yzwx))/3.;"
 "}"
 "float c(vec3 v,vec3 f,vec3 d,vec3 x)"
 "{"
   "vec4 i=p(v-sign(d)*x,x.zxy,x.yzx);"
   "f=fract(f);"
   "vec2 m=f.yz*x.x+f.zx*x.y+f.xy*x.z;"
   "return mix(mix(i.z,i.w,m.x),mix(i.y,i.x,m.x),m.y);"
 "}\n"
 "#define MM 0\n"
 "float d=1.,k=3.,y=1.,a=.67,i=1.,l=50.,b=0.;"
 "vec2 h(vec2 v)"
 "{"
   "return fract(18.5453*sin(v*mat2(127.1,311.7,269.5,183.3)));"
 "}"
 "vec2 w(vec2 v)"
 "{"
   "return-.5+2.*h(v);"
 "}"
 "vec3 u(vec2 v)"
 "{"
   "vec2 f=floor(v),d,x;"
   "float i=1e9,m;"
   "for(int c=0;c<25;c++)"
     "{"
       "vec2 y=f+vec2(c%5-2,c/5-2),t=w(y),r=y-v+t;"
       "m=dot(r,r);"
       "if(m<i)"
         "i=m,d=y-f,x=r;"
     "}"
   "i=1e9;"
   "for(int c=0;c<25;c++)"
     "{"
       "vec2 y=f+d+vec2(c%5-2,c/5-2),t=w(y),s=y-v+t;"
       "if(dot(x-s,x-s)>1e-5)"
         "i=min(i,.5*dot(x+s,normalize(s-x)));"
     "}"
   "return vec3(i,x+v);"
 "}"
 "int M=1;"
 "float o(vec2 v)"
 "{"
   "return fract(sin(dot(v,vec2(127.1,311.7)))*43758.5453123);"
 "}"
 "float g(vec2 v)"
 "{"
   "vec2 f=floor(v),x=fract(v);"
   "x=x*x*(3.-2.*x);"
   "float i=mix(mix(o(f+vec2(0,0)),o(f+vec2(1,0)),x.x),mix(o(f+vec2(0,1)),o(f+vec2(1,1)),x.x),x.y);"
   "return M==0?i:M==1?2.*i-1.:M==2?abs(2.*i-1.):1.-abs(2.*i-1.);"
 "}"
 "vec2 I(vec2 v)"
 "{"
   "vec2 f=vec2(0);"
   "float x=.5;"
   "mat2 d=rot(.37);"
   "for(int i=0;i<6;i++,v*=2.,x/=2.)"
     "v*=d,f+=x*vec2(g(v),g(v+17.7));"
   "return f;"
 "}"
 "vec3 P(vec2 v)"
 "{"
   "vec3 f,x=vec3(0.);"
   "for(float c=0.;c<k;c++)"
     "{"
       "vec2 m=v/vec2(d,1),z=a*I(v/y)*y;"
       "vec3 r=u(m+z);"
       "if(c==0.)"
         "f=r;"
       "float s=r.x;"
       "s=min(1.,l*pow(max(0.,s-b),i));"
       "x+=vec3(1.-s)/exp2(c);"
       "v*=1.5*rot(.37);"
     "}"
   "return x;"
 "}"
 "float I(vec3 v,vec3 f,float d,float x)"
 "{"
   "const float i=6.;"
   "float r=0.;"
   "for(float y=1.;y<i+.5;y++)"
     "{"
       "float m=(y*.75+fract(cos(y)*45758.5453)*.25)/i*d;"
       "r+=(m+c(v-f*m).d)/pow(1.+m,x);"
     "}"
   "return clamp(1.-r/i,0.,1.);"
 "}"
 "vec3 I(vec3 v,vec3 f,float d,mapr x,vec3 m,const bool y)"
 "{"
   "vec3 i=vec3(0.);"
   "if(x.hit)"
     "{"
       "vec3 r=v+f*d,z=-(x.mask*sign(f));"
       "if(x.m==0)"
         "{"
           "vec3 s=m-r;"
           "float t=max(length(s),.001);"
           "s/=t;"
           "float u=1./(1.+t*.2+t*.1),k=n(r+z*.01,s,t),l=c(x.vPos,r,f,x.mask),a=max(0.,dot(z,s)),e=pow(max(0.,dot(reflect(-s,z),-f)),32.);"
           "vec3 M=normalize(s+z);"
           "float b=I(r,z,1.,1.),p=pow(clamp(dot(f,-M),0.,1.),1.),o=p*b;"
           "vec2 g;"
           "vec3 h=abs(z);"
           "if(h.x>h.y&&h.x>h.z)"
             "g=r.yz;"
           "else"
             " if(h.y>h.x&&h.y>h.z)"
               "g=r.xz;"
             "else"
               " if(h.z>h.y&&h.z>h.x)"
                 "g=r.xy;"
           "vec3 w=vec3(.1);"
           "if(_MapIndex==1)"
             "w=vec3(.1,0.,0.);"
           "i=w*(a+.25)+vec3(.7,.9,1.)*e;"
           "if(!y)"
             "{"
               "vec3 C=vec3(.2);"
               "if(_MapIndex==1)"
                 "C=vec3(.2,0.,0.);"
               "i+=C*o;"
               "i+=P(g);"
               "i*=u*l*k;"
             "}"
         "}"
     "}"
   "return i;"
 "}"
 "void main()"
 "{"
   "if(_RenderingTarget==2)"
     "gl_FragColor=vec4(vec3(0.),1.);"
   "else"
     "{"
       "vec2 v=uv*2.-1.;"
       "v.x*=_resolution.x/_resolution.y;"
       "float f=.1,x=t(v)*f-f+_time;"
       "vec3 d=vec3(0.),i=vec3(0.);"
       "if(_MapIndex==0)"
         "d=vec3(0.,0.,1.5+x*20.),i=d+vec3(0.,0.,1.),i.xy+=n(i.z),d.xy+=n(d.z);"
       "else"
         " if(_MapIndex==1)"
           "d=vec3(cos(_time*.1)*5.,-1.,sin(_time*.1)*5.),i=vec3(0.,-2.,0.);"
       "vec3 m=d+vec3(0.,1.,5.);"
       "if(_MapIndex==0)"
         "m.xy+=n(m.z);"
       "float y=1.-.45*length(v),r=0.;"
       "vec3 s=vec3(0.),z=normalize(i-d),a=normalize(cross(vec3(0.,1.,0.),z)),l=normalize(cross(z,a)),e=normalize(v.x*a+v.y*l+y*z),u,k;"
       "bool M;"
       "{"
         "mapr h=c(d,e,false);"
         "vec3 g=(h.vPos-d-.5*sign(e))/e;"
         "r=max(max(g.x,g.y),g.z);"
         "s=I(d,e,r,h,m,false);"
         "k=d+e*r;"
         "u=-(h.mask*sign(e));"
         "M=h.hit;"
       "}"
       "if(M)"
         "{"
           "d=k;"
           "e=clamp(reflect(e,u),vec3(-.5),vec3(.5));"
           "mapr h=c(d,e,true);"
           "vec3 g=(h.vPos-d-.5*sign(e))/e;"
           "float b=max(max(g.x,g.y),g.z);"
           "b=clamp(b,0.,1.);"
           "s+=I(d,e,b,h,m,true);"
         "}"
       "vec3 g=mix(vec3(.96),vec3(.24),-e.y*.5+.5);"
       "if(_MapIndex==0)"
         "s=mix(s,g*sqrt(g)*1.2,smoothstep(0.,.95,r/60.));"
       "else"
         " if(_MapIndex==1)"
           "s=mix(s,g*sqrt(g)*1.2,smoothstep(0.,.95,r/120.));"
       "gl_FragColor=vec4(s,_Alpha);"
     "}"
 "}",

