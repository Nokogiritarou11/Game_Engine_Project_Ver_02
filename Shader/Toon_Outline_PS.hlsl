struct VERTEX_OUT
{
    float4 position : SV_POSITION;
    float4 edgecolor : COLOR;
    float2 texcoord : TEXCOORD0;
};

Texture2D diffuseMap : register(t1);
SamplerState diffuseMapSamplerState : register(s1);

float4 main(VERTEX_OUT input) : SV_TARGET
{
    return float4(input.edgecolor.rgb, diffuseMap.Sample(diffuseMapSamplerState, input.texcoord).a);
}