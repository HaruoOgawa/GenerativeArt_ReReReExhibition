

#version 330
#extension GL_ARB_separate_shader_objects : enable

uniform mat4 MVPMatrix;
uniform mat4 MMatrix;
uniform mat4 VMatrix;
uniform mat4 PMatrix;
uniform float _time;
uniform float _deltaTime;
uniform vec3 _CameraPos;
uniform int _IsMulMatOnVert;
uniform int _IsMOnly;

layout(location=0)in vec3 vertex;
layout(location=1)in vec3 normal;
layout(location=2)in vec2 texcoord;

layout(location=0) out vec2 out_uv;
layout(location=1) out vec4 out_WorldVertexPos;
layout(location=2) out vec4 out_WorldNormal;
layout(location=3) flat out int out_gl_InstanceID;
layout(location = 4) out vec4 out_Color;

#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))

void main(){
	vec4 pos=vec4(vertex,1.0);
	int id=gl_InstanceID;

	if(_IsMulMatOnVert == 1)
	{
		gl_Position=MVPMatrix*pos;
		out_uv=texcoord;
		out_WorldVertexPos=MMatrix * pos;
		out_WorldNormal = normalize(MMatrix * vec4(normal, 0.0));
		out_gl_InstanceID = id;
		out_Color = vec4(1.0);
	}
	else if(_IsMOnly == 1)
	{
		gl_Position=MMatrix*pos;
		out_uv=texcoord;
		out_WorldVertexPos=MMatrix*pos;
		out_WorldNormal=vec4(normalize(normal), 0.0);
		out_gl_InstanceID = id;
		out_Color = vec4(1.0);
	}
	else if(_IsMulMatOnVert == 0)
	{
		gl_Position=pos;
		out_uv=texcoord;
		out_WorldVertexPos=pos;
		out_WorldNormal=vec4(normalize(normal), 0.0);
		out_gl_InstanceID = id;
		out_Color = vec4(1.0);
	}
}

