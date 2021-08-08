cbuffer CbScene : register(b0)
{
	float4x4 viewProjection;
	float4x4 shadowMatrix;
	float4	 lightDirection;
	float3	 lightcolor;
	float	 bias;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 sdwcoord : SHADOW_COORD;
};