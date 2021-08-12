#include "Scene_Constants.hlsli"

struct VERTEX_IN
{
	float3 position;
	float3 normal;
	float3 tangent;
	float2 texcoord;
	float4 bone_weights;
	uint4  bone_indices;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	float4x4 boneTransforms[MAX_BONES];
};

// In
StructuredBuffer<VERTEX_IN> vertex_in : register(t0);
// Out
RWStructuredBuffer<VERTEX> vertex_out : register(u0);

[numthreads(128, 1, 1)]
void main(uint3 dispatch : SV_DispatchThreadID)
{
	int num = dispatch.x;
	float4 pos = float4(vertex_in[num].position, 1.0f);
	float4 nor = float4(vertex_in[num].normal, 0.0f);
	float4 tan = float4(vertex_in[num].tangent, 0.0f);
	float3 p = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	float3 t = { 0, 0, 0 };

	for (int i = 0; i < 4; i++)
	{
		p += (vertex_in[num].bone_weights[i] * mul(boneTransforms[vertex_in[num].bone_indices[i]], pos)).xyz;
		n += (vertex_in[num].bone_weights[i] * mul(boneTransforms[vertex_in[num].bone_indices[i]], nor)).xyz;
		t += (vertex_in[num].bone_weights[i] * mul(boneTransforms[vertex_in[num].bone_indices[i]], tan)).xyz;
	}

	vertex_out[num].position = p;

	vertex_out[num].normal = normalize(n);
	vertex_out[num].tangent = normalize(t);
	vertex_out[num].texcoord = vertex_in[num].texcoord;
}