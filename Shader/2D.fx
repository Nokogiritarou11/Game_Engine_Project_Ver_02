Texture2D tex2d : register(t0);
SamplerState smpState : register(s0);

struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

VS_OUT VSMain(float3 position : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
	VS_OUT vout;
	vout.position = float4(position, 1);
	vout.color = color;
	vout.texcoord = texcoord;
	return vout;
}

float4 PSMain(VS_OUT pin) : SV_TARGET
{
	return tex2d.Sample(smpState, pin.texcoord) * pin.color;
}