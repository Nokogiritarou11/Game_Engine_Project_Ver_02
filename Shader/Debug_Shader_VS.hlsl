#include "Scene_Constants.hlsli"

struct VS_DEBUG_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_DEBUG_OUT main
(
	float3 position : POSITION,
    float4 color : COLOR
)
{
	VS_DEBUG_OUT vout;

	float4 pos = float4(position, 1.0f);
	vout.position = mul(viewProjection, pos);
    vout.color = color;

	return vout;
}