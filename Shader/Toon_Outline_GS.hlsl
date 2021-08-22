#include "Scene_Constants.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 position_out : TEXCOORD2;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 edgecolor : COLOR;
    float2 texcoord : TEXCOORD0;
    float3 sdwcoord : TEXCOORD1;
};

struct VERTEX_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 edgecolor : COLOR;
    float2 texcoord : TEXCOORD0;
    float3 sdwcoord : TEXCOORD1;
};

[maxvertexcount(6)]
void main(triangle VS_OUT input[3], inout TriangleStream<VERTEX_OUT> output)
{
    VERTEX_OUT element;
    for (uint i = 0; i < 3; ++i)
    {
        element.position = input[i].position;
        element.normal = input[i].normal;
        element.tangent = input[i].tangent;
        element.texcoord = input[i].texcoord;
        element.sdwcoord = input[i].sdwcoord;
        element.edgecolor = input[i].edgecolor;
        output.Append(element);
    }
    output.RestartStrip();

	{//—ÖŠsü(— •Ô‚µƒ|ƒŠƒSƒ“ì¬)
        VERTEX_OUT data;

        data.position = input[0].position_out;
        data.normal = input[0].normal;
        data.tangent = input[0].tangent;
        data.texcoord = input[0].texcoord;
        data.sdwcoord = input[0].sdwcoord;
        data.edgecolor = input[0].edgecolor;
        data.edgecolor.a = -1;
        output.Append(data);

        data.position = input[2].position_out;
        data.normal = input[2].normal;
        data.tangent = input[2].tangent;
        data.texcoord = input[2].texcoord;
        data.sdwcoord = input[2].sdwcoord;
        data.edgecolor = input[2].edgecolor;
        data.edgecolor.a = -1;
        output.Append(data);

        data.position = input[1].position_out;
        data.normal = input[1].normal;
        data.tangent = input[1].tangent;
        data.texcoord = input[1].texcoord;
        data.sdwcoord = input[1].sdwcoord;
        data.edgecolor = input[1].edgecolor;
        data.edgecolor.a = -1;
        output.Append(data);
    }

    output.RestartStrip();
}