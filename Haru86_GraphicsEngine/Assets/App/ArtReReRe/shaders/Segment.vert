R"(

#version 430
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

struct STrailData
{
	vec4 Pos;
	vec4 Rotate;
	vec4 Scale;
	vec4 Color;
	vec4 Velocity;
	vec4 TargetPos;
	vec4 Data;
};

layout(std430, binding = 0) buffer TrailDataBuffer
{
	STrailData trailData[];
} rw_TrailDataBuffer;

// 
struct SSegmentData 
{
	vec4 Pos;
	vec4 Rotate;
	vec4 Scale;
	vec4 Color;
	int  TrailIndex;
	int  SegmentIndex;
	int  Padding0;
	int  Padding1;
};

layout(std430, binding = 1) buffer SegmentDataBuffer
{
	SSegmentData segmentData[];
} rw_SegmentDataBuffer;

layout(location=0)in vec3 vertex;
layout(location=1)in vec3 normal;
layout(location=2)in vec2 texcoord;

layout(location=0) out vec2 out_uv;
layout(location=1) out vec4 out_WorldVertexPos;
layout(location=2) out vec4 out_WorldNormal;
layout(location=3) flat out int out_gl_InstanceID;
layout(location=4) out vec4 out_Color;

#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))

void main(){
	vec4 pos = vec4(vertex, 1.0);
	int id = gl_InstanceID;
	int TrailID = rw_SegmentDataBuffer.segmentData[id].TrailIndex;
	STrailData trailData = rw_TrailDataBuffer.trailData[TrailID];

	gl_Position = pos;
	out_uv = texcoord;
	out_WorldVertexPos = pos;
	out_WorldNormal=vec4(normalize(normal), 0.0);
	out_gl_InstanceID = id;
	out_Color = rw_SegmentDataBuffer.segmentData[id].Color;
}

)"