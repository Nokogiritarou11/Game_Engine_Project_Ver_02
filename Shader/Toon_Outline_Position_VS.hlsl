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


VERTEX_OUT main(uint index : SV_VertexID)
{
    VERTEX_OUT vout;

    float4 pos = float4(vertex[index].position, 1.0f);
    float pos_vec = dot(normalize(vertex[index].origin_position), normalize(vertex[index].origin_normal)) < 0 ? -1 : 1;
    vout.position = mul(viewProjection, pos + float4(normalize(vertex[index].origin_position) * pos_vec, 0) * outline_size * 2);
    vout.edgecolor = float4(outline_color, 1.0f);
    vout.texcoord = vertex[index].texcoord;

    return vout;
}