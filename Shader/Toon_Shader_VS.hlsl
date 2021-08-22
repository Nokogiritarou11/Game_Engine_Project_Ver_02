#include "Scene_Constants.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 position_out : TEXCOORD2;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 edgecolor : COLOR;
    float2 texcoord : TEXCOORD0;
    float3 sdwcoord : TEXCOORD1;
};

// í∏ì_èÓïÒ
StructuredBuffer<VERTEX> vertex : register(t0);
cbuffer CbOutline : register(b1)
{
    float3 outline_color = { 0.2f, 0.2f, 0.2f };
    float outline_size = 0.0f;
};


VS_OUT main(uint index : SV_VertexID)
{
    VS_OUT vout;

    float4 pos = float4(vertex[index].position, 1.0f);
    vout.position = mul(viewProjection, pos);
    vout.normal = float4(vertex[index].normal, 0.0f);
    vout.tangent = float4(vertex[index].tangent, 0.0f);
    vout.texcoord = vertex[index].texcoord;

    float4 shadow_pos = mul(shadowMatrix, pos);
    shadow_pos /= shadow_pos.w;
    vout.sdwcoord = shadow_pos.xyz;

    vout.position_out = mul(viewProjection, pos + vout.normal * outline_size);
    vout.edgecolor = float4(outline_color, 1.0f);

    return vout;
}