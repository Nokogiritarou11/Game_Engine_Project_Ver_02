#include "Scene_Constants.hlsli"

struct VS_SHADOW_OUT
{
    float4 position : SV_POSITION;
    float4 depth : TEXCOORD; //ê[ìxíl
};

// í∏ì_èÓïÒ
StructuredBuffer<VERTEX> vertex : register(t0);

VS_SHADOW_OUT main(uint index : SV_VertexID)
{
    VS_SHADOW_OUT vout;

    vout.position = mul(viewProjection, float4(vertex[index].position, 1.0f));
    vout.depth = vout.position;

    return vout;
}