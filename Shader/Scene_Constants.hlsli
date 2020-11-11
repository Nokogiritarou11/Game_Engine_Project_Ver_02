cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
	row_major float4x4	shadowMatrix;
	float4				lightDirection;
	float3				lightcolor;
	float				bias;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 sdwcoord : SHADOW_COORD;
};