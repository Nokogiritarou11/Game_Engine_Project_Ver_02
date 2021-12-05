#include "Scene_Constants.hlsli"

struct VERTEX_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD0;
    float3 sdwcoord : TEXCOORD1;
};

cbuffer CbColor : register(b1)
{
    float4 main_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

Texture2D shadowMap : register(t0);
SamplerState ShadowMapSamplerState : register(s0);

Texture2D diffuseMap : register(t1);
SamplerState diffuseMapSamplerState : register(s1);

Texture2D normalMap : register(t2);
SamplerState normalMapSamplerState : register(s2);

Texture2D rampMap : register(t3);
SamplerState RampMapSamplerState : register(s3);

float4 main(VERTEX_OUT input) : SV_TARGET
{
    float4 outcolor;

    float4 normal_map_colour = normalMap.Sample(normalMapSamplerState, input.texcoord);
    normal_map_colour = (normal_map_colour * 2.0) - 1.0;
    normal_map_colour.w = 0;
	// transform to world space from tangent space
	//                 |Tx Ty Tz|
	// normal = |x y z||Bx By Bz|
	//                 |Nx Ny Nz|
    float3 N = input.normal.xyz;
    float3 T = input.tangent.xyz;
    float3 B = normalize(cross(N, T));
    N = normalize((normal_map_colour.x * T) + (normal_map_colour.y * B) + (normal_map_colour.z * N));

    float3 L = normalize(-lightDirection.xyz);

    float diffuse_factor = saturate(dot(N, L));
    diffuse_factor = diffuse_factor * 0.5 + 0.5f;
    diffuse_factor = diffuse_factor * diffuse_factor;

    float4 mapdiff = diffuseMap.Sample(diffuseMapSamplerState, input.texcoord) * main_color * float4(lightcolor, 1);

    float4 main_level = mapdiff * rampMap.Sample(RampMapSamplerState, float2(diffuse_factor, 0));

    float2 d = shadowMap.Sample(ShadowMapSamplerState, input.sdwcoord.xy).rg;
    // •ªŽU‚ÌŒvŽZ
    float d_sq = d.x * d.x; // E(x)^2
    float variance = max(bias, d.y - d_sq); // ƒÐ^2 = E(x^2) - E(x^2)

    // Šm—¦ã‚ÌÅ‘å’l‚ÌŽZo
    float md = d.x - input.sdwcoord.z;
    float p_max = saturate(variance / (variance + (md * md)));

    p_max = ReduceLightBleeding(p_max, 0.65f);

    // ‰e‚ÌF
    float3 shadowColor = max(input.sdwcoord.z > d.x, lerp(float3(0.8f, 0.8f, 0.8f), 1.0f, p_max));

    outcolor.rgb = main_level.rgb * shadowColor;
    outcolor.a = mapdiff.a;

    return outcolor;
}