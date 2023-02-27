/* File generated with Shader Minifier 1.3.1
 * http://www.ctrl-alt-test.fr
 */

// E:\CppDev\Art_ReReRe\Haru86_GraphicsEngine\GraphicsEngine\Graphics\Shader\StandardRenderBoard_Vert.h
"#version 330\n"
 "uniform mat4 MVPMatrix,MMatrix,VMatrix,PMatrix;"
 "uniform float _time,_deltaTime;"
 "uniform vec2 _resolution;"
 "layout(location=0) in vec3 vertex;"
 "layout(location=1) in vec3 normal;"
 "layout(location=2) in vec2 texcoord;"
 "out vec2 uv;"
 "void main()"
 "{"
   "vec4 v=vec4(vertex,1.);"
   "gl_Position=v;"
   "uv=texcoord;"
 "}",

