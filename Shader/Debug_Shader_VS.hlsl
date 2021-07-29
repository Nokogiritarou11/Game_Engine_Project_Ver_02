#include "Scene_Constants.hlsli"
#include "Debug_Shader.hlsli"

VS_DEBUG_OUT VSMain(
	float3 position : POSITION,
    float3 color : COLOR
)
{
    VS_DEBUG_OUT vout;

    float4 pos = float4(position, 1.0f);
    vout.position = mul(pos, viewProjection);
    vout.color = float4(color, 1.0f);

    return vout;
}