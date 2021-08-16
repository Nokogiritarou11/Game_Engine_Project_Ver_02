#include "Gaussian_Shader.hlsli"

float4 main(PSInput input) : SV_TARGET
{
    float4 output = { 0.0f, 0.0f, 0.0f, 0.0f };

    output += weight_0.x * (ColorTex.Sample(ColorSmp, input.TexCoord0) + ColorTex.Sample(ColorSmp, input.TexCoord7 + float2(offset.x, 0.0f)));
    output += weight_0.y * (ColorTex.Sample(ColorSmp, input.TexCoord1) + ColorTex.Sample(ColorSmp, input.TexCoord6 + float2(offset.x, 0.0f)));
    output += weight_0.z * (ColorTex.Sample(ColorSmp, input.TexCoord2) + ColorTex.Sample(ColorSmp, input.TexCoord5 + float2(offset.x, 0.0f)));
    output += weight_0.w * (ColorTex.Sample(ColorSmp, input.TexCoord3) + ColorTex.Sample(ColorSmp, input.TexCoord4 + float2(offset.x, 0.0f)));

    output += weight_1.x * (ColorTex.Sample(ColorSmp, input.TexCoord4) + ColorTex.Sample(ColorSmp, input.TexCoord3 + float2(offset.x, 0.0f)));
    output += weight_1.y * (ColorTex.Sample(ColorSmp, input.TexCoord5) + ColorTex.Sample(ColorSmp, input.TexCoord2 + float2(offset.x, 0.0f)));
    output += weight_1.z * (ColorTex.Sample(ColorSmp, input.TexCoord6) + ColorTex.Sample(ColorSmp, input.TexCoord1 + float2(offset.x, 0.0f)));
    output += weight_1.w * (ColorTex.Sample(ColorSmp, input.TexCoord7) + ColorTex.Sample(ColorSmp, input.TexCoord0 + float2(offset.x, 0.0f)));

    return output;
}