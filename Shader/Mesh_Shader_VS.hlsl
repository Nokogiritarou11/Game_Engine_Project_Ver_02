#include "Scene_Constants.hlsli"

cbuffer CbMesh : register(b1)
{
	float4x4 world;
};

VS_OUT VSMain(
	float3 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 bone_weights : WEIGHTS,
	uint4 bone_indices : BONES
)
{
	VS_OUT vout;

	float4 pos = float4(position, 1.0f);
	float4 nor = float4(normal, 0.0f);
	float4 tan = float4(tangent, 0.0f);

	vout.position = mul(viewProjection, mul(world, pos));

	vout.normal = normalize(mul(world, nor));
	vout.tangent = normalize(mul(world, tan));
	vout.texcoord = texcoord;

	vout.sdwcoord = mul(shadowMatrix, mul(world, pos) + vout.normal * bias);
	return vout;
}