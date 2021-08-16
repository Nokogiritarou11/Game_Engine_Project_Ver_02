#include "Gaussian_Shader.hlsli"

PSInput main(VSInput input)
{
    PSInput output;

    output.Position = float4(input.Position, 1.0f);

    output.TexCoord0 = input.TexCoord + float2(0.0f, -1.0f / size.y);
    output.TexCoord1 = input.TexCoord + float2(0.0f, -3.0f / size.y);
    output.TexCoord2 = input.TexCoord + float2(0.0f, -5.0f / size.y);
    output.TexCoord3 = input.TexCoord + float2(0.0f, -7.0f / size.y);
    output.TexCoord4 = input.TexCoord + float2(0.0f, -9.0f / size.y);
    output.TexCoord5 = input.TexCoord + float2(0.0f, -11.0f / size.y);
    output.TexCoord6 = input.TexCoord + float2(0.0f, -13.0f / size.y);
    output.TexCoord7 = input.TexCoord + float2(0.0f, -15.0f / size.y);

    return output;
}