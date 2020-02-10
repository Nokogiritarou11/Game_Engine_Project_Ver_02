
Texture2D tex2d : register(t0);
SamplerState smpState : register(s0);

#define MAX_BONES 32
cbuffer CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 world_view_projection;
	row_major float4x4 world;
	float4 material_color;
	float4 light_direction;
	// UNIT.21
	row_major float4x4 bone_transforms[MAX_BONES];
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VS_OUT VSMain(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 bone_weights : WEIGHTS, uint4 bone_indices : BONES)
{
	VS_OUT vout;

	float3 p = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		p += (bone_weights[i] * mul(position, bone_transforms[bone_indices[i]])).xyz;
		n += (bone_weights[i] * mul(float4(normal.xyz, 0), bone_transforms[bone_indices[i]])).xyz;
	}
	position = float4(p, 1.0f);
	normal = float4(n, 0.0f);
	vout.position = mul(position, world_view_projection);
	normal.w = 0;
	float4 N = normalize(mul(normal, world));
	float4 L = normalize(light_direction);

	//vout.color = material_color * max(0, dot(L, N)) * 0.5f + 0.5f;
	vout.color = material_color * max(0.5f, dot(L, N));
	vout.color.a = material_color.a;
	vout.texcoord = texcoord;
	return vout;
}

float4 PSMain(VS_OUT pin) : SV_TARGET
{
	return tex2d.Sample(smpState, pin.texcoord) * pin.color;
}