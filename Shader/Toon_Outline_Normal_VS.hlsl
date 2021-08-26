#include "Scene_Constants.hlsli"

struct VERTEX_OUT
{
    float4 position : SV_POSITION;
    float4 edgecolor : COLOR;
    float2 texcoord : TEXCOORD0;
};

// í∏ì_èÓïÒ
StructuredBuffer<VERTEX> vertex : register(t0);

cbuffer CbOutline : register(b1)
{
    float3 outline_color = { 0.1f, 0.1f, 0.1f };
    float outline_size = 0.002f;
};

Texture2D OutlineMask : register(t1);
SamplerState OutlineMaskSamplerState : register(s1);

VERTEX_OUT main(uint index : SV_VertexID)
{
    VERTEX_OUT vout;

    vout.texcoord = vertex[index].texcoord;

    float4 pos = float4(vertex[index].position, 1.0f);
    float outline_width = outline_size * OutlineMask.SampleLevel(OutlineMaskSamplerState, vout.texcoord, 0).r;
    vout.position = mul(viewProjection, pos + float4(vertex[index].normal, 0) * outline_width);
    vout.edgecolor = float4(outline_color, 1.0f);

    return vout;
}