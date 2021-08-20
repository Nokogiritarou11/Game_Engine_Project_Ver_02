#include "Scene_Constants.hlsli"

struct VERTEX_IN
{
    float3 position;
    float3 normal;
    float3 tangent;
    float2 texcoord;
    float4 bone_weights;
    uint4 bone_indices;
};

cbuffer CbMesh : register(b1)
{
    float4x4 world;
};

// In
StructuredBuffer<VERTEX_IN> vertex_in : register(t0);
// Out
RWStructuredBuffer<VERTEX> vertex_out : register(u0);

[numthreads(64, 1, 1)]
void main(uint3 dispatch : SV_DispatchThreadID)
{
    int num = dispatch.x;
    float4 pos = float4(vertex_in[num].position, 1.0f);
    float4 nor = float4(vertex_in[num].normal, 0.0f);
    float4 tan = float4(vertex_in[num].tangent, 0.0f);

    vertex_out[num].position = mul(world, pos).xyz;

    vertex_out[num].normal = normalize(mul(world, nor)).xyz;
    vertex_out[num].tangent = normalize(mul(world, tan)).xyz;
    vertex_out[num].texcoord = vertex_in[num].texcoord;
}