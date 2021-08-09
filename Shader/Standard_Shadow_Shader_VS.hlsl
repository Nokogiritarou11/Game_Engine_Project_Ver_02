#include "Scene_Constants.hlsli"

struct VS_SHADOW_OUT
{
	float4 position : SV_POSITION;
};

VS_SHADOW_OUT main(
	float3 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD
)
{
	VS_SHADOW_OUT vout;

	float4 pos = float4(position, 1.0f);
	vout.position = mul(viewProjection, pos);

	return vout;
}