struct VERTEX_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

VERTEX_OUT main
(
	float3 position : POSITION,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR
)
{
	VERTEX_OUT vout;
	vout.position = float4(position, 1);
	vout.color = color;
	vout.texcoord = texcoord;
	return vout;
}