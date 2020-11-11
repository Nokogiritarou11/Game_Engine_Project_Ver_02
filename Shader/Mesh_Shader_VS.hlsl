#include "Scene_Constants.hlsli"

cbuffer CbMesh : register(b1)
{
    row_major float4x4 world;
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

    vout.position = mul(pos, mul(world, viewProjection));

    vout.normal = normalize(mul(nor, world));
    vout.tangent = normalize(mul(tan, world));
    vout.texcoord = texcoord;

    vout.sdwcoord = mul(vout.position, shadowMatrix);
    return vout;
}