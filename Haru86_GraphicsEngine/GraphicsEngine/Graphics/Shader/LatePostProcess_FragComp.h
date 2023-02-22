/* File generated with Shader Minifier 1.3.1
 * http://www.ctrl-alt-test.fr
 */

// E:\CppDev\GradDemo\Haru86_GraphicsEngine\GraphicsEngine\Graphics\Shader\LatePostProcess_Frag.h
"#version 410\n"
 "in vec2 uv;"
 "uniform float _time,_deltaTime;"
 "uniform vec2 _resolution;"
 "uniform sampler2D _SrcTexture,_NormalMap,_DepthMapPolygone,_DepthMapRaymarch,_DepthMapMixed;"
 "uniform float _frameResolusion;"
 "uniform mat4 VPMatrix,InvVPMatrix;"
 "uniform vec3 _WorldCameraPos,_WorldCameraCenter;"
 "uniform int _UseVignette;"
 "uniform float _VignetteRadius,_VignetteLateRadius,_VignetteBrightness,_VignetteBlendRate;"
 "uniform int _UseWhiteNoise;"
 "uniform float _WhiteNoisePower;"
 "uniform int _UseFilmFilter;"
 "uniform float _FilmWidthReduction;"
 "uniform int _UseWave;"
 "uniform float _WaveSpeed,_WaveWidth;"
 "uniform int _UseWhiteFade;"
 "uniform float _WhiteFadeVal;"
 "uniform int _UseGridWave;"
 "uniform float _WavePower;"
 "uniform int _UseWaveCustom,_NotUseFraRes,_UseMixer;"
 "uniform sampler2D polygon_frameTexture,polygon_depthTexture,raymarching_frameTexture,raymarching_depthTexture;"
 "float f(vec2 e)"
 "{"
   "return fract(sin(dot(e,vec2(12.9898,78.233)))*43758.5453);"
 "}"
 "vec3 e(vec3 e)"
 "{"
   "vec2 v=uv*2.-1.;"
   "float f=1.1+(1.-_VignetteRadius)*4.-_VignetteLateRadius*.5,r=length(v)*f;"
   "e=mix(e,vec3(0.),r);"
   "e.xyz+=vec3(_VignetteBrightness);"
   "return e;"
 "}"
 "vec3 e(vec3 e,vec2 v)"
 "{"
   "return e+f(vec2(v.x,v.y)+_time)*_WhiteNoisePower*vec3(1.);"
 "}"
 "vec3 t(vec3 e)"
 "{"
   "vec2 v=uv*2.-1.;"
   "float f=.2-_FilmWidthReduction;"
   "if(abs(v.y)>1.-f)"
     "e=vec3(0.);"
   "return e;"
 "}"
 "vec3 v(vec3 e)"
 "{"
   "return e+=vec3(1.)*_WhiteFadeVal,e;"
 "}"
 "vec3 f(vec3 e,vec2 v)"
 "{"
   "float f=_UseWaveCustom==1?_WavePower:(sin(_time*2.)+1.)*.5+.1;"
   "v.y+=.05*sin(_time*100.)*f;"
   "v.x+=.01*sin(_time*50.)*f;"
   "vec4 i=vec4(1.,.5,.25,0.);"
   "float s=.08*f;"
   "vec2 r=vec2(0.,0.)-(v-vec2(.5,.5))*2.,y=v+r*s*i.x,m=v+r*s*i.y,p=v+r*s*i.z;"
   "e.x+=texture(_SrcTexture,y).x;"
   "e.y+=texture(_SrcTexture,m).y;"
   "e.z+=texture(_SrcTexture,p).z;"
   "return e;"
 "}"
 "void main()"
 "{"
   "vec2 r=_NotUseFraRes==1?uv:uv*_frameResolusion;"
   "vec3 s=texture(_SrcTexture,r).xyz;"
   "if(_UseGridWave==1)"
     "s=f(s,r);"
   "if(_UseWhiteFade==1)"
     "s=v(s);"
   "if(_UseWhiteNoise==1)"
     "s=e(s,r);"
   "if(_UseVignette==1)"
     "s=mix(s,e(s),_VignetteBlendRate);"
   "if(_UseFilmFilter==1)"
     "s=t(s);"
   "if(_UseMixer==1)"
     "{"
       "vec4 i=texture(polygon_frameTexture,r);"
       "vec3 m=texture(polygon_depthTexture,r).xyz,p=texture(raymarching_frameTexture,r).xyz,y=texture(raymarching_depthTexture,r).xyz;"
       "if(m.x<=y.x&&i.w>0.)"
         "s=mix(p,i.xyz,i.w);"
       "else"
         " s=p;"
     "}"
   "gl_FragColor=vec4(s,1.);"
 "}",

