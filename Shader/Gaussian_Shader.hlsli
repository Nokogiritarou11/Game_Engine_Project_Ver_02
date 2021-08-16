Texture2D ColorTex : register(t0);
SamplerState ColorSmp : register(s0);

cbuffer CbGauss : register(b1)
{
    float4 weight_0;
    float4 weight_1;
    float2 offset;
    float2 size;
};

struct VSInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord0 : TEXCOORD0;
    float2 TexCoord1 : TEXCOORD1;
    float2 TexCoord2 : TEXCOORD2;
    float2 TexCoord3 : TEXCOORD3;
    float2 TexCoord4 : TEXCOORD4;
    float2 TexCoord5 : TEXCOORD5;
    float2 TexCoord6 : TEXCOORD6;
    float2 TexCoord7 : TEXCOORD7;
};