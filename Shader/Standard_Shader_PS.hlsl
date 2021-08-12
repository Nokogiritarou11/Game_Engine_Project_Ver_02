#include "Scene_Constants.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 sdwcoord : SHADOW_COORD;
};

cbuffer CbColor : register(b1)
{
    float4 materialColor;
};

Texture2D shadowMap : register(t0);
SamplerComparisonState ShadowMapSamplerState : register(s0);

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

	//float diffuse_factor = max(dot(L, N), 0);


	// シャドウマップの深度値と比較する.
    float3 shadowCoord = pin.sdwcoord.xyz / pin.sdwcoord.w;
    /*
	// 最大深度傾斜を求める.
    float maxDepthSlope = max(abs(ddx(shadowCoord.z)), abs(ddy(shadowCoord.z)));
    float shadowThreshold = 1.0f; // シャドウにするかどうかの閾値です.
    float slopeScaledBias = 0.01f; // 深度傾斜.
    float depthBiasClamp = 0.01f; // バイアスクランプ値.
    float3 shadowColor = float3(0.75f, 0.75f, 0.75f);

    float shadowBias = bias + slopeScaledBias * maxDepthSlope;
    shadowBias = min(shadowBias, depthBiasClamp);
    shadowThreshold = shadowMap.SampleCmpLevelZero(ShadowMapSamplerState, shadowCoord.xy, shadowCoord.z - shadowBias);*/
    float3 shadowColor = float3(0.75f, 0.75f, 0.75f);
    float shadowThreshold = shadowMap.SampleCmpLevelZero(ShadowMapSamplerState, shadowCoord.xy, shadowCoord.z);
    shadowColor = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold);

    float3 Ka = mapdiff.rgb * (1 - diffuse_factor) * 0.8;
    float3 Kd = mapdiff.rgb * diffuse_factor * 1.5f;
    outcolor.rgb = (Ka + Kd) * shadowColor;
	//outcolor.rgb = ((mapdiff.rgb * 0.7f) + (diffuse_factor * 0.3f)) * shadowColor;
    outcolor.a = mapdiff.a;

    return outcolor;
}