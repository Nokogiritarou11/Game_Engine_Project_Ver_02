struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
	float4 sdwcoord : SHADOW_COORD;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
};

cbuffer CbColor : register(b2)
{
	float4				materialColor;
};
