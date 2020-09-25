struct VS_IN
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 bone_weights : WEIGHTS;
	uint4 bone_indices : BONES;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float3 texcoord : TEXCOORD;
};

cbuffer CONSTANT_BUFFER : register(b1)
{
	column_major float4x4 world;
	float4 color;
};