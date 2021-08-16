#include "Scene_Constants.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD0;
    float3 sdwcoord : TEXCOORD1;
};

VS_OUT main(VERTEX input)
{
    VS_OUT vout;

    float4 pos = float4(input.position, 1.0f);

    vout.position = mul(viewProjection, pos);
    vout.normal = float4(input.normal, 0.0f);
    vout.tangent = float4(input.tangent, 0.0f);
    vout.texcoord = input.texcoord;

    float4 shadow_pos = mul(shadowMatrix, pos + vout.normal * bias);
    shadow_pos /= shadow_pos.w;
    //shadow_pos.y = -shadow_pos.y;
    //shadow_pos.xy = 0.5f * shadow_pos.xy + 0.5f;
    vout.sdwcoord = shadow_pos.xyz;

    return vout;
}