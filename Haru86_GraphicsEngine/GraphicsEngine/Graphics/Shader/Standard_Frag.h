#version 430
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec2 in_uv;
layout(location=1) in vec4 in_WorldVertexPos;
layout(location=2) in vec4 in_WorldNormal;
layout(location=3) flat in int  in_gl_InstanceID;
layout(location = 4) in vec4 in_Color;

out vec4 out_Color;

uniform int _UseColor;
uniform vec4 _Color;
uniform int _Use2FColor;
uniform int _UseLighting;
uniform vec3 _LightDir;
uniform vec3 _LightPos;
uniform vec3 _WorldCameraPos;
uniform int _UseEnvColor;
uniform vec4 _EnvColor;

uniform int _UseMainTex;
uniform int _UseMainEditorTex;
uniform sampler2D _MainTex;
uniform int _UseMainCube;
uniform samplerCube _MainCube;

uniform int _UseRim;
uniform int _IsRimMul;
uniform vec4 _RimColor;
uniform float _RimPower;
uniform float _RimMulVal;

uniform float _EditorOffset;
uniform float _ShrinkRate;
uniform float _FontSize;
uniform float _wCharH;

void main(){
	vec4 col=vec4(vec3(0.0),1.0);

	// 環境光
	vec4 envColor = vec4(0.0, 0.0, 0.0, 1.0);
	if (_UseEnvColor == 1)
	{
		envColor = _EnvColor;
	}

	// ベースカラー
	if(_UseColor == 1)
	{
		col=_Color;
	}
	else if (_Use2FColor == 1)
	{
		col = in_Color;
		envColor = in_Color;
	}
	else if(_UseMainTex == 1) // テクスチャサンプリング
	{
		vec4 TexCol = texture(_MainTex,vec2(in_uv.x,1.0-in_uv.y));
		col = _Color * TexCol.r;
	}
	else if(_UseMainEditorTex == 1) // テクスチャサンプリング
	{
		vec2 st = vec2(in_uv.x, in_uv.y);
		st.y += -1.0;
		st *= _ShrinkRate;
		st.y += 1.0;

		vec4 TexCol = texture(_MainTex,vec2(st.x, st.y + _EditorOffset));
		col = TexCol;

		float h = (_wCharH - 1.0) * _ShrinkRate + 1.0;
		float w = 0.02 * _ShrinkRate, s = _FontSize * _ShrinkRate;
		if(st.y < h && st.x >= w) col.a = 0.0;
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

	// ライティング
	if(_UseLighting == 1)
	{
		vec3 lightDir= _LightDir;
		//vec3 lightDir=normalize(_LightPos-in_WorldVertexPos.xyz);
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

	// リムライティング
	if(_UseRim == 1)
	{
		vec3 viewDir= -normalize(in_WorldVertexPos.xyz-_WorldCameraPos);
		float rimval = 1.0 - dot(in_WorldNormal.xyz, viewDir);
		rimval = pow(rimval,_RimPower);
		rimval = max(0.0, rimval);
		rimval*=_RimMulVal;

		if(_IsRimMul == 1)
		{
			col *=_RimColor * rimval;
		}
		else
		{
			col=_RimColor * rimval;
		}
	}

	//col.rgb = in_WorldNormal.xyz*0.5+0.5;

	if(col.a == 0.0) { discard; }

	//gl_FragColor=col;
	out_Color=col;
}