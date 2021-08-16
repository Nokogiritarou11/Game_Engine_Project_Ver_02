#include "Scene_Constants.hlsli"

struct VS_SHADOW_OUT
{
    float4 position : SV_POSITION;
    float4 depth : TEXCOORD; //ê[ìxíl
};

float4 main(VS_SHADOW_OUT input) : SV_TARGET
{
    float c = input.depth.z / input.depth.w;
    return float4(c, c * c, 0, 1);
}