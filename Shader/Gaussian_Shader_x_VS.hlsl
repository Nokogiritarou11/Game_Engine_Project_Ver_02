#include "Gaussian_Shader.hlsli"

PSInput main(VSInput input)
{
    PSInput output;

    output.Position = float4(input.Position, 1.0f);

    output.TexCoord0 = input.TexCoord + float2(-1.0f / size.x, 0.0f);
    output.TexCoord1 = input.TexCoord + float2(-3.0f / size.x, 0.0f);
    output.TexCoord2 = input.TexCoord + float2(-5.0f / size.x, 0.0f);
    output.TexCoord3 = input.TexCoord + float2(-7.0f / size.x, 0.0f);
    output.TexCoord4 = input.TexCoord + float2(-9.0f / size.x, 0.0f);
    output.TexCoord5 = input.TexCoord + float2(-11.0f / size.x, 0.0f);
    output.TexCoord6 = input.TexCoord + float2(-13.0f / size.x, 0.0f);
    output.TexCoord7 = input.TexCoord + float2(-15.0f / size.x, 0.0f);

    return output;
}