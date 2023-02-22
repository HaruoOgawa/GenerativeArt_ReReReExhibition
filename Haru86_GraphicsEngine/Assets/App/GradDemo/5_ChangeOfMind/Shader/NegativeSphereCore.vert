R"(

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

layout(location=0)in vec3 vertex;
layout(location=1)in vec3 normal;
layout(location=2)in vec2 texcoord;

layout(location=0) out vec2 out_uv;
layout(location=1) out vec4 out_WorldVertexPos;
layout(location=2) out vec4 out_WorldNormal;

#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))

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

void main(){
	vec4 pos=vec4(vertex,1.0);
    float off = fbm3(pos.xyz*2.0 + _time * 2.0 + 100.0, 5.0, 0.75, 0.25) * 4.0 -1.0;
	pos.xyz += normal.xyz * off;

	if(_IsMulMatOnVert == 1)
	{
		gl_Position=MVPMatrix*pos;
		out_uv=texcoord;
		out_WorldVertexPos=MMatrix * pos;
		out_WorldNormal=MMatrix * vec4(normalize(normal), 0.0);
	}
	else if(_IsMulMatOnVert == 0)
	{
		gl_Position=pos;
		out_uv=texcoord;
		out_WorldVertexPos=pos;
		out_WorldNormal=vec4(normalize(normal), 0.0);
	}
}

)"