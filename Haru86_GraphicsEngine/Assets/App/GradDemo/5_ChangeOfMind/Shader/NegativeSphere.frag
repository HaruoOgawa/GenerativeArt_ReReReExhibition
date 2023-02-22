R"(

#version 330
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec2 in_uv;
layout(location=1) in vec4 in_WorldVertexPos;
layout(location=2) in vec4 in_WorldNormal;
layout(location=3) flat in float in_Displacement;
layout(location=4) in vec4 in_AnchorPos;

uniform int _UseColor;
uniform vec4 _Color;
uniform int _UseLighting;
uniform vec3 _LightDir;
uniform vec3 _LightPos;
uniform vec3 _WorldCameraPos;
uniform int _UseEnvColor;
uniform vec4 _EnvColor;

uniform int _UseMainTex;
uniform sampler2D _MainTex;
uniform int _UseMainCube;
uniform samplerCube _MainCube;
uniform float _Alpha;

void main(){
	vec4 col=vec4(vec3(0.0),1.0);

	// ベースカラー
	if(_UseColor == 1)
	{
		col=_Color;
	}
	else if(_UseMainTex == 1) // テクスチャサンプリング
	{
		col=texture(_MainTex,in_uv);
	}
	else if(_UseMainCube == 1)
	{
		vec3 viewdir = -normalize(_WorldCameraPos-in_WorldVertexPos.xyz);
		vec3 rpdir = normalize(reflect(viewdir,in_WorldNormal.xyz));
		col.rgb=texture(_MainCube,rpdir).rgb;

		//col=vec4(rpdir*0.5+0.5,1.0);
	}
	else
	{
		col=vec4(1.0);
		//col=vec4(in_uv.x,in_uv.y,0.0,1.0);
	}

	// 環境光
	vec4 envColor = vec4(0.0,0.0,0.0,1.0);
	if(_UseEnvColor == 1)
	{
		envColor = _EnvColor;
	}

	// ライティング
	if(_UseLighting == 1)
	{
		//vec3 lightDir=normalize(_LightDir);
		vec3 lightDir=normalize(_LightPos-in_WorldVertexPos.xyz);
		float diff=max(0.0,dot(in_WorldNormal.xyz,lightDir));
		col.rgb*=diff;

		col.rgb+=envColor.rgb;

		vec3 viewDir= -1.0*normalize(in_WorldVertexPos.xyz-_WorldCameraPos);
		vec3 halfDir=normalize(viewDir + lightDir);
		float spec=pow( max(0.0,dot(in_WorldNormal.xyz,halfDir)) , 60.0);
		// とてつもなく、少数部が細かい(桁が多い)数が来るとfloat Textureの精度が足りなくなってMSAA使用時に白いドットのノイズが出てしまうのでその対策
		spec = min(1.0,spec);
		col.rgb+=vec3(1.0)*spec;
	}

	//col = mix(col, vec4(1.0 * 3.0,0.0,0.0,1.0), clamp(in_Displacement-0.35, 0.0, 1.0));

	float mixval = distance(in_WorldVertexPos.xyz, in_AnchorPos.xyz) / 1.5;
	col = mix(col, vec4(1.0 * 3.0,0.0,0.0,1.0), clamp(mixval, 0.0, 1.0));
	col.a *= _Alpha;

	gl_FragColor=col;
}

)"