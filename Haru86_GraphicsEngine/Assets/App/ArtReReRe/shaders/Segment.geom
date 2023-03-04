R"(

#version 430
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

// Uniform 
uniform float _time;
uniform float _deltaTime;
uniform int _SegmentNum;
uniform mat4 VMatrix;
uniform mat4 PMatrix;
uniform float _Tickness;

uniform vec4 _WallHalfSize;

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

bool IsOutterWall(vec4 p)
{
	//float Shrink = 0.75;
	float Shrink = 0.95;

	if(abs(p.x) > _WallHalfSize.x * Shrink){ return true; }
	if(abs(p.y) > _WallHalfSize.y * Shrink){ return true; }
	if(abs(p.z) > _WallHalfSize.z * Shrink){ return true; }

	return false;
}

void main()
{
	//
	int id0 = in_gl_InstanceID[0];
	SSegmentData data0 = rw_SegmentDataBuffer.segmentData[id0];

	int id1 = (data0.SegmentIndex == _SegmentNum - 1)? id0 : id0 + 1;
	SSegmentData data1 = rw_SegmentDataBuffer.segmentData[id1];

	// 
	vec3 dir = normalize(data1.Pos.xyz - data0.Pos.xyz);
	vec4 n = vec4(normalize(cross(dir, vec3(0.0,1.0, 0.0))), 0.0);
	vec3 mDir = normalize(cross(n.xyz, dir));

	//
	vec4 pos0 = data1.Pos + vec4(mDir * _Tickness, 0.0);
	vec4 pos1 = data1.Pos - vec4(mDir * _Tickness, 0.0);
	vec4 pos2 = data0.Pos + vec4(mDir * _Tickness, 0.0);
	vec4 pos3 = data0.Pos - vec4(mDir * _Tickness, 0.0);

	//
	float Alpha = (IsOutterWall(data1.Pos))? 0.0 : 1.0;
	Alpha *= float(data1.SegmentIndex) / float(_SegmentNum - 1);
	vec4 Color = vec4(in_Color[0].rgb, Alpha);

	//
	gl_Position = PMatrix * VMatrix * pos2;
	out_uv = in_uv[0];
	out_WorldVertexPos = pos2;
	out_WorldNormal = n;
	out_gl_InstanceID = in_gl_InstanceID[0];
	out_Color = Color;
	EmitVertex();

	gl_Position = PMatrix * VMatrix * pos1;
	out_uv = in_uv[0];
	out_WorldVertexPos = pos1;
	out_WorldNormal = n;
	out_gl_InstanceID = in_gl_InstanceID[0];
	out_Color = Color;
	EmitVertex();

	gl_Position = PMatrix * VMatrix * pos3;
	out_uv = in_uv[0];
	out_WorldVertexPos = pos3;
	out_WorldNormal = n;
	out_gl_InstanceID = in_gl_InstanceID[0];
	out_Color = Color;
	EmitVertex();

	EndPrimitive();

	//
	gl_Position = PMatrix * VMatrix * pos0;
	out_uv = in_uv[0];
	out_WorldVertexPos = pos0;
	out_WorldNormal = n;
	out_gl_InstanceID = in_gl_InstanceID[0];
	out_Color = Color;
	EmitVertex();
	
	gl_Position = PMatrix * VMatrix * pos1;
	out_uv = in_uv[0];
	out_WorldVertexPos = pos1;
	out_WorldNormal = n;
	out_gl_InstanceID = in_gl_InstanceID[0];
	out_Color = Color;
	EmitVertex();

	gl_Position = PMatrix * VMatrix * pos2;
	out_uv = in_uv[0];
	out_WorldVertexPos = pos2;
	out_WorldNormal = n;
	out_gl_InstanceID = in_gl_InstanceID[0];
	out_Color = Color;
	EmitVertex();

	EndPrimitive();
}

)"