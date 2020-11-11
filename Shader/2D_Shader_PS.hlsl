struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

Texture2D tex2d : register(t1);
SamplerState smpState : register(s1);

float4 PSMain(VS_OUT pin) : SV_TARGET
{
	return tex2d.Sample(smpState, pin.texcoord) * pin.color;
}