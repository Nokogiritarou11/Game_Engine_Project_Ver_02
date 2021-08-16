#include "Scene_Constants.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD0;
    float3 sdwcoord : TEXCOORD1;
};

cbuffer CbColor : register(b1)
{
    float4 materialColor;
};

Texture2D shadowMap : register(t0);
SamplerState ShadowMapSamplerState : register(s0);

Texture2D diffuseMap : register(t1);
SamplerState diffuseMapSamplerState : register(s1);

Texture2D normalMap : register(t3);
SamplerState normalMapSamplerState : register(s3);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 outcolor;

    float4 mapdiff = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * materialColor;

    float4 normal_map_colour = normalMap.Sample(normalMapSamplerState, pin.texcoord);
    normal_map_colour = (normal_map_colour * 2.0) - 1.0;
    normal_map_colour.w = 0;
	// transform to world space from tangent space
	//                 |Tx Ty Tz|
	// normal = |x y z||Bx By Bz|
	//                 |Nx Ny Nz|
    float3 N = pin.normal.xyz;
    float3 T = pin.tangent.xyz;
    float3 B = normalize(cross(N, T));
    N = normalize((normal_map_colour.x * T) + (normal_map_colour.y * B) + (normal_map_colour.z * N));

    float3 L = normalize(-lightDirection.xyz);

    float diffuse_factor = saturate(dot(N, L));
    diffuse_factor = diffuse_factor * diffuse_factor;

	// シャドウマップの深度値と比較する.
    /*
    float3 shadowCoord = pin.sdwcoord.xyz / pin.sdwcoord.w;
    float3 shadowColor = float3(0.75f, 0.75f, 0.75f);
    float shadowThreshold = shadowMap.SampleCmpLevelZero(ShadowMapSamplerState, shadowCoord.xy, shadowCoord.z);
    shadowColor = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold);
    */
    float2 d = shadowMap.Sample(ShadowMapSamplerState, pin.sdwcoord.xy).rg;
    // 分散の計算
    float d_sq = d.x * d.x; // E(x)^2
    float variance = d.y - d_sq; // σ^2 = E(x^2) - E(x^2)

    // 確率上の最大値の算出
    float p_max = saturate((variance) / (variance + ((pin.sdwcoord.z - d.x) * (pin.sdwcoord.z - d.x))));
    // 影の色
    float3 shadowColor = max(pin.sdwcoord.z < d.x, lerp(float3(0.75f, 0.75f, 0.75f), 1.0f, p_max));

    float3 Ka = mapdiff.rgb * (1 - diffuse_factor) * 0.8;
    float3 Kd = mapdiff.rgb * diffuse_factor * 1.5f;
    outcolor.rgb = (Ka + Kd) * shadowColor;
    outcolor.a = mapdiff.a;

    return outcolor;
}