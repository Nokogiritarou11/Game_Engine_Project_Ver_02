#include "Scene_Constants.hlsli"

struct VS_SHADOW_OUT
{
	float4 position : SV_POSITION;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
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
	float3 p = { 0, 0, 0 };

	for (int i = 0; i < 4; i++)
	{
		p += (bone_weights[i] * mul(pos, boneTransforms[bone_indices[i]])).xyz;
	}

	vout.position = mul(float4(p, 1.0f), viewProjection);

	return vout;
}