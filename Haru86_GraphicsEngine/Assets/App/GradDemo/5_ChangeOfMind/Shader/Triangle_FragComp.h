/* File generated with Shader Minifier 1.3.1
 * http://www.ctrl-alt-test.fr
 */

// E:\CppDev\GradDemo\Haru86_GraphicsEngine\Assets\App\GradDemo\5_ChangeOfMind\Shader\Triangle_Frag.h
"#version 330\n"
 "uniform float _time;"
 "uniform vec2 _resolution;"
 "uniform float _RenderingTarget;"
 "uniform vec3 _WorldCameraPos,_WorldCameraCenter;"
 "uniform float _LeaveStartTime,_Alpha;"
 "in vec2 uv;\n"
 "#define pi 3.14159265\n"
 "#define rot(a)mat2(cos(a),-sin(a),sin(a),cos(a))\n"
 "#define dmin 0.001\n"
 "#define tmax 30.0\n"
 "#define ldir normalize(vec3(1.0,1.0,-1.0))\n"
 "vec3 v;"
 "int m;"
 "float f;struct mapr{float d;bool hit;int m;vec3 vPos;vec3 mask;float i;float tvalue;float acc;vec3 vCol;float flash;};"
 "void s(inout mapr v,float y,int f,bool a)"
 "{"
   "if(a)"
     "{"
       "if(y<v.d)"
         "v=mapr(y,false,f,vec3(0.),vec3(0.),0.,0.,0.,vec3(0.),0.);"
       "if(v.d<dmin)"
         "v.hit=true;"
     "}"
   "else"
     " v.d=max(v.d,-y);"
 "}"
 "vec3 n(vec3 v,vec3 a,vec3 f,vec3 e)"
 "{"
   "return v+=e,v.yz*=rot(a.x),v.xz*=rot(a.y),v.xy*=rot(a.z),v*=a,v;"
 "}"
 "vec2 n(vec2 v,float y)"
 "{"
   "float f=2.*pi/y,a=atan(v.x,v.y)-.5*f;"
   "a=mod(a,f)-.5*f;"
   "return length(v.xy)*vec2(cos(a),sin(a));"
 "}"
 "float s(vec3 v,vec3 a)"
 "{"
   "vec3 m=abs(v),f=max(a-m,0.);"
   "return length(max(m-a,0.))-min(min(f.x,f.y),f.z);"
 "}"
 "vec4 t(vec3 v,vec3 a,float e,vec3 f)"
 "{"
   "vec4 m=vec4(v,1.);"
   "for(int i=0;i<12;i++)"
     "{"
       "if(m.x+m.y<0.)"
         "m.xy=-m.yx,f.z+=1.;"
       "if(m.x+m.z<0.)"
         "m.xz=-m.zx,f.y+=1.;"
       "if(m.z+m.y<0.)"
         "m.zy=-m.yz,f.x+=1.;"
       "m*=e;"
       "m.xyz+=a*(1.-e);"
     "}"
   "return vec4(f,s(m.xyz,vec3(1.5))/m.w);"
 "}"
 "float t(vec3 v,float a)"
 "{"
   "return v=abs(v),length(max(v-vec3(a),0.));"
 "}"
 "float n(vec3 v,vec3 a,float m)"
 "{"
   "vec4 f=vec4(v.xyz,1.);"
   "int e=10;"
   "for(int i=0;i<e;i++)"
     "{"
       "if(f.x+f.y<0.)"
         "f.xy=-f.yx;"
       "if(f.x+f.z<0.)"
         "f.xz=-f.zx;"
       "if(f.y+f.z<0.)"
         "f.yz=-f.yz;"
       "f*=m;"
       "f.xyz-=a*(m-1.);"
     "}"
   "return t(f.xyz,1.)/f.w;"
 "}"
 "mapr n(vec3 v)"
 "{"
   "mapr f;"
   "f.d=1e3;"
   "f.hit=false;"
   "f.m=-1;"
   "{"
     "vec3 a=v;"
     "a.xz=n(a.xz,32.);"
     "a.x-=2.1;"
     "a=abs(a)-.5;"
     "a.yz*=rot(.5);"
     "a.xy*=rot(.15);"
     "a.xz*=rot(.5);"
     "a.yz*=rot(pi/2.);"
     "a=abs(a)-.25;"
     "a.z=abs(a.z)-.5;"
     "a.x=abs(a.x)-.75;"
     "a.y=abs(a.y)-.75;"
     "a.x=abs(a.x)-.65;"
     "a.x=dot(a,a);"
     "float y=1.4;"
     "a=mod(a,y)-y*.5;"
     "s(f,n(a,vec3(1.24),1.8),0,true);"
     "s(f,abs(v.y)-.5,0,false);"
     "vec4 m=t(a,vec3(1),1.8,vec3(0.));"
     "float e=m.w;"
     "vec3 i=1.-.1*m.xyz-.3;"
     "i*=exp(-2.5*e)*2.;"
     "f.vCol+=i;"
   "}"
   "{"
     "vec3 a=n(v,vec3(1.5),vec3(0.),vec3(0.));"
     "s(f,n(a,vec3(1.24),1.8),1,true);"
     "vec4 m=t(a,vec3(1),1.8,vec3(0.));"
     "float y=m.w;"
     "vec3 i=1.-.1*m.xyz-.3;"
     "i*=exp(-2.5*y)*2.;"
     "f.vCol+=i;"
   "}"
   "return f;"
 "}"
 "vec3 s(vec3 a)"
 "{"
   "vec2 f=vec2(.001,0.);"
   "return normalize(vec3(n(a+f.xyy).d-n(a-f.xyy).d,n(a+f.yxy).d-n(a-f.yxy).d,n(a+f.yyx).d-n(a-f.yyx).d));"
 "}"
 "mapr r(vec3 v,vec3 a)"
 "{"
   "mapr f;"
   "float m=0.,y=0.,i=0.,e=0.;"
   "f.vCol=vec3(0.);"
   "f.flash=0.;"
   "for(;++m<64.;)"
     "{"
       "f=n(v+a*(y+=f.d*.75));"
       "if(f.d<dmin||y>tmax)"
         "break;"
       "i+=exp(-3.*f.d);"
       "if(mod(distance(vec3(0.),v+a*y)-_time*10.,10.)<1.)"
         "e+=exp(-3.*f.d);"
     "}"
   "f.i=m;"
   "f.tvalue=y;"
   "f.acc=i;"
   "f.flash=e;"
   "return f;"
 "}"
 "float e(vec3 a,vec3 v)"
 "{"
   "float f=1.,i=0.;"
   "for(int y=0;y<5;y++)"
     "{"
       "float m=.15+float(y)*.15,e=n(v*m+a).d;"
       "i+=(m-e)*f;"
       "f*=.5;"
     "}"
   "return clamp(1.-i,0.,1.);"
 "}"
 "void main()"
 "{"
   "if(_RenderingTarget==2)"
     "gl_FragColor=vec4(vec3(0.),1.);"
   "else"
     "{"
       "vec2 a=uv*2.-1.;"
       "a.x*=_resolution.x/_resolution.y;"
       "vec3 v=vec3(cos(_time)*3.,0.,sin(_time)*3.),m=vec3(0.,0.,0.),i=vec3(0.),y=normalize(m-v),e=normalize(cross(vec3(0.,1.,0.),y)),x=normalize(cross(y,e)),z=normalize(a.x*e+a.y*x+y);"
       "f=64.;"
       "mapr c=r(v,z);"
       "if(c.m==0)"
         "{"
           "vec3 d=v+z*c.tvalue,t=s(d);"
           "i=vec3(.615,.8,.88)*10./c.i*c.acc*.1+c.vCol*c.acc*.01;"
           "i+=vec3(1.)*c.flash*.015;"
         "}"
       "else"
         " if(c.m==1)"
           "{"
             "vec3 d=v+z*c.tvalue,t=s(d);"
             "i=vec3(.615,.8,.88)*10./c.i+c.vCol*c.acc*.05;"
             "i+=vec3(1.)*c.flash*.015;"
           "}"
       "gl_FragColor=vec4(i,_Alpha);"
     "}"
 "}",
