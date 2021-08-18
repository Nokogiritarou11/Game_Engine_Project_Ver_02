#include "Scene_Constants.hlsli"

struct VS_SHADOW_OUT
{
    float4 position : SV_POSITION;
    float4 depth : TEXCOORD; //ê[ìxíl
};

float4 main(VS_SHADOW_OUT input) : SV_TARGET
{
    float c = input.depth.z / input.depth.w;
    float dx = ddx(c);
    float dy = ddy(c);
    return float4(c, c * c + 0.25f * (dx * dx + dy * dy), 0, 1);
}