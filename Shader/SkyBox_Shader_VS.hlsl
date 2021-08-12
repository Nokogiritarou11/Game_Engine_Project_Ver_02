#include "Scene_Constants.hlsli"

struct VS_SKY_OUT
{
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD;
};

cbuffer CONSTANT_BUFFER : register(b1)
{
    float4x4 world;
};

VS_SKY_OUT main(float3 position : POSITION)
{
    VS_SKY_OUT vout;

    vout.position = mul(mul(viewProjection, world), float4(position, 1.0f));
    vout.texcoord = normalize(position.xyz);

    return vout;
}