

#version 410

in vec2 uv;

uniform float _time;
uniform float _deltaTime;
uniform vec2 _resolution;
uniform sampler2D _SrcTexture;
uniform sampler2D _NormalMap;
uniform sampler2D _DepthMapPolygone;
uniform sampler2D _DepthMapRaymarch;
uniform sampler2D _DepthMapMixed;
uniform float _frameResolusion;
uniform mat4 VPMatrix;
uniform mat4 InvVPMatrix;
uniform vec3 _WorldCameraPos;
uniform vec3 _WorldCameraCenter;
uniform int _UseVignette;
uniform float _VignetteRadius;
uniform float _VignetteLateRadius;
uniform float _VignetteBrightness;
uniform float _VignetteBlendRate;
uniform int _UseWhiteNoise;
uniform float _WhiteNoisePower;
uniform int _UseFilmFilter;
uniform float _FilmWidthReduction;
uniform int _UseWave;
uniform float _WaveSpeed;
uniform float _WaveWidth;
uniform int _UseWhiteFade;
uniform float _WhiteFadeVal;
uniform int _UseGridWave;
uniform float _WavePower;
uniform int _UseWaveCustom;
uniform int _NotUseFraRes;
uniform int _UseMixer;
uniform sampler2D polygon_frameTexture;
uniform sampler2D polygon_depthTexture;
uniform sampler2D raymarching_frameTexture;
uniform sampler2D raymarching_depthTexture;

float rand(vec2 st)
{
    return fract(sin(dot(st, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 Vignette(in vec3 col)
{
	vec2 st = uv*2.0-1.0;
	
	float w = 1.1+(1.0-_VignetteRadius)*4.0 - _VignetteLateRadius*0.5;
	
	float d = (length(st))*w;
	col = mix(col,vec3(0.0),d);
	//col*=d*0.5*(1.0+_VignetteBrightness);
	//col*=d*0.5*(1.0+9.0*_VignetteBrightness);
	//col = mix(col, max(vec3(0.0), col), min(1.0, (_VignetteBrightness - 0.25) * 2.0));
	col.rgb+=vec3(_VignetteBrightness);

	return col;
}

// ホワイトノイズ
vec3 WhiteNoise(in vec3 col, vec2 st){ return col + vec3(1.0) * (rand(vec2(st.x,st.y)+_time) * _WhiteNoisePower  ); }

vec3 DrawFilmFilter(in vec3 col)
{
	vec2 st = uv*2.0-1.0;

	float w = 0.2 - _FilmWidthReduction;
	if(abs(st.y) > (1.0-w)) col = vec3(0.0);

	return col;
}

vec3 DrawWhite(in vec3 col)
{
	col+=vec3(1.0)*_WhiteFadeVal;
	return col;
}

vec3 GridWave(in vec3 col, vec2 st)
{
	float val=(_UseWaveCustom==1)? _WavePower : (sin(_time*2.0)+1.0)*0.5+0.1;

	st.y+=0.05*sin(_time*100.0)*val;
	st.x+=0.01*sin(_time*50.0)*val;

	vec4 _GapOffsetX=vec4(1.0,0.5,0.25,0.0);
	vec4 _GapOffsetY=vec4(0.02,0.03,0.03,0.0)*6.0;
	float _gapOffsetPower=0.08*val;
				
	vec2 vecFromCenter=vec2(0.0,0.0)-(st-vec2(0.5,0.5))*2.0;

    vec2 st_R=st+vecFromCenter*_gapOffsetPower*_GapOffsetX.r;
    vec2 st_G=st+vecFromCenter*_gapOffsetPower*_GapOffsetX.g;
    vec2 st_B=st+vecFromCenter*_gapOffsetPower*_GapOffsetX.b;

    col.r+=texture(_SrcTexture,st_R).r;
    col.g+=texture(_SrcTexture,st_G).g;
    col.b+=texture(_SrcTexture,st_B).b;

	return col;
}

void main(){
	vec2 st=(_NotUseFraRes==1)? uv : uv*_frameResolusion;
	vec3 col=texture(_SrcTexture,st).rgb;
	
	if(_UseGridWave == 1) col = GridWave(col, st);

	if(_UseWhiteFade == 1) col = DrawWhite(col);
	if(_UseWhiteNoise == 1) col = WhiteNoise(col, st);

	if(_UseVignette == 1) col = mix(col, Vignette(col), _VignetteBlendRate);
	if(_UseFilmFilter == 1) col = DrawFilmFilter(col);
	if(_UseMixer == 1)
	{
		vec4 polygonCol=texture(polygon_frameTexture,st);
		vec3 polygonDepth=texture(polygon_depthTexture,st).rgb;
		vec3 raymarchingCol=texture(raymarching_frameTexture,st).rgb;
		vec3 raymarchingDepth=texture(raymarching_depthTexture,st).rgb;
		if(polygonDepth.r<=raymarchingDepth.r && polygonCol.a > 0.0){col = mix(raymarchingCol, polygonCol.rgb, polygonCol.a);}
		else{col= raymarchingCol;}
	}
	
	gl_FragColor=vec4(col,1.0);
}

