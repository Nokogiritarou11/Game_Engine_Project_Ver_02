Texture2D tex2d : register(t0);
SamplerState smpState : register(s0);

cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
	float4				lightDirection;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
	float4				materialColor;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VS_OUT VSMain(
	float3 position : POSITION,
	float3 normal : NORMAL,
	float2 texcoord : TEXCOORD,
	float4 bone_weights : WEIGHTS,
	uint4 bone_indices : BONES
)
{
	VS_OUT vout;

	float4 pos = float4(position, 1.0f);
	float4 nor = float4(normal, 0);
	float3 p = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		//p += (bone_weights[i] * mul(pos, bone_transforms[0])).xyz;
		p += (bone_weights[i] * mul(pos, bone_transforms[bone_indices[i]])).xyz;
		//n += (bone_weights[i] * mul(nor, bone_transforms[0])).xyz;
		n += (bone_weights[i] * mul(nor, bone_transforms[bone_indices[i]])).xyz;
	}
	pos = float4(p, 1.0f);
	nor = float4(n, 0.0f);
	vout.position = mul(pos, world_view_projection);
	nor.w = 0;
	float4 N = normalize(mul(nor, world));
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