#include "Scene_Constants.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 sdwcoord : SHADOW_COORD;
};

VS_OUT main(VERTEX input)
{
    VS_OUT vout;

    float4 pos = float4(input.position, 1.0f);

    vout.position = mul(viewProjection, pos);
    vout.normal = float4(input.normal, 0.0f);
    vout.tangent = float4(input.tangent, 0.0f);
    vout.texcoord = input.texcoord;
    vout.sdwcoord = mul(shadowMatrix, pos + vout.normal * bias);

    return vout;
}