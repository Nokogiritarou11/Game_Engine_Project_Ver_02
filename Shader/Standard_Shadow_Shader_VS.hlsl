#include "Scene_Constants.hlsli"

struct VS_SHADOW_OUT
{
    float4 position : SV_POSITION;
    float4 depth : TEXCOORD; //ê[ìxíl
};

VS_SHADOW_OUT main(VERTEX input)
{
    VS_SHADOW_OUT vout;

    vout.position = mul(viewProjection, float4(input.position, 1.0f));
    vout.depth = vout.position;

    return vout;
}