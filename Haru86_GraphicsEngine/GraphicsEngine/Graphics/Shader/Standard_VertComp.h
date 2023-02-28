/* File generated with Shader Minifier 1.3.1
 * http://www.ctrl-alt-test.fr
 */

// E:\CppDev\Art_ReReRe\Haru86_GraphicsEngine\GraphicsEngine\Graphics\Shader\Standard_Vert.h
"#version 330\n"
 "#extension GL_ARB_separate_shader_objects:enable\n"
 "uniform mat4 MVPMatrix,MMatrix,VMatrix,PMatrix;"
 "uniform float _time,_deltaTime;"
 "uniform vec3 _CameraPos;"
 "uniform int _IsMulMatOnVert,_IsMOnly;"
 "layout(location=0) in vec3 vertex;"
 "layout(location=1) in vec3 normal;"
 "layout(location=2) in vec2 texcoord;"
 "layout(location=0) out vec2 out_uv;"
 "layout(location=1) out vec4 out_WorldVertexPos;"
 "layout(location=2) out vec4 out_WorldNormal;"
 "layout(location=3) flat out int out_gl_InstanceID;"
 "layout(location=4) out vec4 out_Color;\n"
 "#define rot(a)mat2(cos(a),-sin(a),sin(a),cos(a))\n"
 "void main()"
 "{"
   "vec4 v=vec4(vertex,1.);"
   "int n=gl_InstanceID;"
   "if(_IsMulMatOnVert==1)"
     "gl_Position=MVPMatrix*v,out_uv=texcoord,out_WorldVertexPos=MMatrix*v,out_WorldNormal=normalize(MMatrix*vec4(normal,0.)),out_gl_InstanceID=n,out_Color=vec4(1.);"
   "else"
     " if(_IsMOnly==1)"
       "gl_Position=MMatrix*v,out_uv=texcoord,out_WorldVertexPos=MMatrix*v,out_WorldNormal=vec4(normalize(normal),0.),out_gl_InstanceID=n,out_Color=vec4(1.);"
     "else"
       " if(_IsMulMatOnVert==0)"
         "gl_Position=v,out_uv=texcoord,out_WorldVertexPos=v,out_WorldNormal=vec4(normalize(normal),0.),out_gl_InstanceID=n,out_Color=vec4(1.);"
 "}",

