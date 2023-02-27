R"(

#version 430
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout(line_strip, max_vertices = 6) out;

// Uniform 
uniform float _time;
uniform float _deltaTime;
uniform int _SegmentNum;
uniform mat4 VMatrix;
uniform mat4 PMatrix;
// 
struct SSegmentData 
{
	vec4 Pos;
	vec4 Rotate;
	vec4 Scale;
	int  TrailIndex;
	int  SegmentIndex;
	int  Padding0;
	int  Padding1;
};

layout(std430, binding = 1) buffer SegmentDataBuffer
{
	SSegmentData segmentData[];
} rw_SegmentDataBuffer;

layout(location=0) in vec2 in_uv[];
layout(location=1) in vec4 in_WorldVertexPos[];
layout(location=2) in vec4 in_WorldNormal[];
layout(location=3) flat in int in_gl_InstanceID[];
layout(location=4) in vec4 in_Color[];

layout(location=0) out vec2 out_uv;
layout(location=1) out vec4 out_WorldVertexPos;
layout(location=2) out vec4 out_WorldNormal;
layout(location=3) flat out int out_gl_InstanceID;
layout(location=4) out vec4 out_Color;

void main()
{
	//
	int id0 = in_gl_InstanceID[0];
	SSegmentData data0 = rw_SegmentDataBuffer.segmentData[id0];

	int id1 = (data0.SegmentIndex == _SegmentNum - 1)? id0 : id0 + 1;
	SSegmentData data1 = rw_SegmentDataBuffer.segmentData[id1];

	//
	gl_Position = PMatrix * VMatrix * data0.Pos;
	out_uv = in_uv[0];
	out_WorldVertexPos = data0.Pos;
	out_WorldNormal = in_WorldNormal[0];
	out_gl_InstanceID = in_gl_InstanceID[0];
	out_Color = in_Color[0];
	EmitVertex();
	
	gl_Position = PMatrix * VMatrix * data1.Pos;
	out_uv = in_uv[0];
	out_WorldVertexPos = data1.Pos;
	out_WorldNormal = in_WorldNormal[0];
	out_gl_InstanceID = in_gl_InstanceID[0];
	out_Color = in_Color[0];
	EmitVertex();

	EndPrimitive();
}

)"