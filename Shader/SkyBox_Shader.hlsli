struct VS_SKY_OUT
{
	float4 position : SV_POSITION;
	float3 texcoord : TEXCOORD;
};

cbuffer CONSTANT_BUFFER : register(b1)
{
	column_major float4x4 world;
	float4 color;
};