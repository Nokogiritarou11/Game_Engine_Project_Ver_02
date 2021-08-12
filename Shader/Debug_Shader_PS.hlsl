#include "Scene_Constants.hlsli"

struct VS_DEBUG_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(VS_DEBUG_OUT pin) : SV_TARGET
{
    float4 outcolor = pin.color;
    return outcolor;
}

