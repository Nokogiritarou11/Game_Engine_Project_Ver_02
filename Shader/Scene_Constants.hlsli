cbuffer CbScene : register(b0)
{
	float4x4 viewProjection;
	float4x4 shadowMatrix;
	float4	 lightDirection;
	float3	 lightcolor;
	float	 bias;
};

struct VERTEX
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
};