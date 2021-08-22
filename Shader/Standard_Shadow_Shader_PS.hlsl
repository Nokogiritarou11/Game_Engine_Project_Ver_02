#include "Scene_Constants.hlsli"

struct VS_SHADOW_OUT
{
    float4 position : SV_POSITION;
    float4 depth : TEXCOORD0; //ê[ìxíl
    float2 texcoord : TEXCOORD1;
};

Texture2D alphaMap : register(t1);
SamplerState alphaMapSamplerState : register(s1);

float4 main(VS_SHADOW_OUT input) : SV_TARGET
{
    float c = input.depth.z / input.depth.w;
    float dx = ddx(c);
    float dy = ddy(c);
    float alpha = alphaMap.Sample(alphaMapSamplerState, input.texcoord).a;
    return float4(c, c * c + 0.25f * (dx * dx + dy * dy), 0, alpha);
}