cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
	row_major float4x4	shadowMatrix;
	float4				lightDirection;
	float3				lightcolor;
	float				bias;
};