#include "SkyBox_Shader.hlsli"
#include "Scene_Constants.hlsli"

VS_SKY_OUT VSMain
(
    float3 position : POSITION,
    float3 normal : NORMAL,
    float2 texcoord : TEXCOORD,
    float4 bone_weights : WEIGHTS,
    uint4 bone_indices : BONES
)
{
    VS_SKY_OUT vout;

    vout.position = mul(float4(position, 1.0f), mul(world, viewProjection));
    vout.texcoord = normalize(position.xyz);

    return vout;
}