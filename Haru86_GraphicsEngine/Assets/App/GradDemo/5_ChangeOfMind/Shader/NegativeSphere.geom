R"(


#version 330
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout(triangle_strip,max_vertices=24) out;

layout(location=0) in vec2 in_uv[];
layout(location=1) in vec4 in_WorldVertexPos[];
layout(location=2) in vec4 in_WorldNormal[];

layout(location=0) out vec2 out_uv;
layout(location=1) out vec4 out_WorldVertexPos;
layout(location=2) out vec4 out_WorldNormal;
layout(location=3) flat out float out_Displacement;
layout(location=4) out vec4 out_AnchorPos;

uniform mat4 MVPMatrix;
uniform mat4 MMatrix;
uniform mat4 VMatrix;
uniform mat4 PMatrix;
uniform float _time;
uniform float _deltaTime;
uniform vec3 _CameraPos;

#define pi 3.14159265

// noise
float rand3(vec3 p)
{
    return fract( sin(dot(p,vec3(12.9898,78.233,128.852)))*43758.5453123 );
}

float vnoise(vec3 p)
{
    vec3 ip = floor(p);
    vec3 fp = fract(p);
    
    float a = rand3(ip + vec3(0.0,0.0,0.0));
    float b = rand3(ip + vec3(1.0,0.0,0.0));
    float c = rand3(ip + vec3(0.0,1.0,0.0));
    float d = rand3(ip + vec3(1.0,1.0,0.0));
    float e = rand3(ip + vec3(0.0,0.0,1.0));
    float f = rand3(ip + vec3(1.0,0.0,1.0));
    float g = rand3(ip + vec3(0.0,1.0,1.0));
    float h = rand3(ip + vec3(1.0,1.0,1.0));
    
    vec3 u = fp*fp*(3.0-2.0*fp);
    
    return mix(
        mix( mix(a,b,u.x), mix(c,d,u.x), u.y),
        mix( mix(e,f,u.x), mix(g,h,u.x), u.y),
        u.z
    );
}

float fbm3(vec3 p,float num,float freq,float amp)
{
    float n = 0.0;
    
    for(float i=0.0;i<num;i++)
    {
        n+=pow(0.5,i)*amp*vnoise(p*pow(2.0,i)*freq);
    }
    
    return n;
}

// main
void main()
{
	//
	vec4 normal = in_WorldNormal[0];
	vec4 bionormal = vec4(normalize(cross( normal.xyz , vec3(0.0, 1.0, 0.0) )), 0.0);
	vec4 tangent = vec4(normalize(cross( normal.xyz,bionormal.xyz )), 0.0);
	vec4 pos0 = gl_in[0].gl_Position;

	//
	float h = fbm3(pos0.xyz*0.5 + _time * 3.0 + 100.0, 5.0, 0.5, 0.25) * 1.5;
	
	//
	float off = fbm3(pos0.xyz*2.0 + _time * 2.0 + 100.0, 5.0, 0.75, 0.25) * 4.0 -1.0;
	pos0.xyz += normal.xyz * off;

	// 
	for(float n=0.0; n < 3.0; n++)
	{
		float ba = (2.0*pi/3.0) * n; // BaseAngle 120.0*n
		float hr = 0.05; // hex radius
		vec4 pos1 = pos0 + hr * normalize( tangent*cos(ba-pi/3.0) + bionormal*sin(ba-pi/3.0) );
		vec4 pos2 = pos0 + hr * normalize( tangent*cos(ba) + bionormal*sin(ba) );
		vec4 pos3 = pos0 + hr * normalize( tangent*cos(ba+pi/3.0) + bionormal*sin(ba+pi/3.0) );

		// ’ê–Ê‚ð\’z
		gl_Position = MVPMatrix * pos0;
		out_uv = in_uv[0];
		out_WorldVertexPos = MMatrix * pos0;
		out_WorldNormal = MMatrix * in_WorldNormal[0];
		out_Displacement = h;
		out_AnchorPos = pos0;
		EmitVertex();

		gl_Position = MVPMatrix * pos1;
		out_uv = in_uv[0];
		out_WorldVertexPos = MMatrix * pos1;
		out_WorldNormal = MMatrix * in_WorldNormal[0];
		out_Displacement = h;
		out_AnchorPos = pos0;
		EmitVertex();

		gl_Position = MVPMatrix * pos3;
		out_uv = in_uv[0];
		out_WorldVertexPos = MMatrix * pos3;
		out_WorldNormal = MMatrix * in_WorldNormal[0];
		out_Displacement = h;
		out_AnchorPos = pos0;
		EmitVertex();

		gl_Position = MVPMatrix * pos2;
		out_uv = in_uv[0];
		out_WorldVertexPos = MMatrix * pos2;
		out_WorldNormal = MMatrix * in_WorldNormal[0];
		out_Displacement = h;
		out_AnchorPos = pos0;
		EmitVertex();

		EndPrimitive();

		// ë‚ª‚Á‚Ä‚é•”•ª‚ð\’z
		if(h > 0.4)
		{
			vec4 pos4 = pos0 + h * normal;

			gl_Position = MVPMatrix * pos4;
			out_uv = in_uv[0];
			out_WorldVertexPos = MMatrix * pos4;
			out_WorldNormal = MMatrix * in_WorldNormal[0];
			out_Displacement = h;
			out_AnchorPos = pos0;
			EmitVertex();

			gl_Position = MVPMatrix * pos1;
			out_uv = in_uv[0];
			out_WorldVertexPos = MMatrix * pos1;
			out_WorldNormal = MMatrix * in_WorldNormal[0];
			out_Displacement = h;
			out_AnchorPos = pos0;
			EmitVertex();

			gl_Position = MVPMatrix * pos3;
			out_uv = in_uv[0];
			out_WorldVertexPos = MMatrix * pos3;
			out_WorldNormal = MMatrix * in_WorldNormal[0];
			out_Displacement = h;
			out_AnchorPos = pos0;
			EmitVertex();

			gl_Position = MVPMatrix * pos2;
			out_uv = in_uv[0];
			out_WorldVertexPos = MMatrix * pos2;
			out_WorldNormal = MMatrix * in_WorldNormal[0];
			out_Displacement = h;
			out_AnchorPos = pos0;
			EmitVertex();

			EndPrimitive();
		}
	}
}

)"