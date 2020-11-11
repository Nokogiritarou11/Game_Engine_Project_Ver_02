#include "Scene_Constants.hlsli"

struct VS_SHADOW_OUT
{
    float4 position : SV_POSITION;
};

cbuffer CbMesh : register(b1)
{
    column_major float4x4 world;
};

VS_SHADOW_OUT VSMain(
	float3 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 bone_weights : WEIGHTS,
	uint4 bone_indices : BONES
)
{
    VS_SHADOW_OUT vout;

    float4 pos = float4(position, 1.0f);

    vout.position = mul(pos, mul(world, viewProjection));

    return vout;
}