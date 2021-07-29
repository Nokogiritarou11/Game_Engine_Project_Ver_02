#include "Scene_Constants.hlsli"
#include "Debug_Shader.hlsli"

float4 PSMain(VS_DEBUG_OUT pin) : SV_TARGET
{
    float4 outcolor = pin.color;
    return outcolor;
}

