#include "Scene_Constants.hlsli"
#include "Debug_Shader.hlsli"
cbuffer CbDebug : register(b1)
{
	float4 color_alpha;
};

VS_DEBUG_OUT main(
	float3 position : POSITION,
    float3 color : COLOR
)
{
	VS_DEBUG_OUT vout;

	float4 pos = float4(position, 1.0f);
	vout.position = mul(viewProjection, pos);
	vout.color = float4(color, color_alpha.w);

	return vout;
}