#include "Shader\\Standard_Shader.hlsli"
#include "Shader\\Scene_Constants.hlsli"

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

	for (int i = 0; i < 4; i++)
	{
		p += (bone_weights[i] * mul(pos, boneTransforms[bone_indices[i]])).xyz;
		n += (bone_weights[i] * mul(float4(nor.xyz, 0), boneTransforms[bone_indices[i]])).xyz;
	}

	vout.position = mul(float4(p, 1.0f), viewProjection);

	float3 N = normalize(n);
	float3 L = normalize(-lightDirection.xyz);
	float d = dot(L, N);
	vout.color.xyz = materialColor.xyz * max(0.5f, d);
	vout.color.w = materialColor.w;
	vout.texcoord = texcoord;

	return vout;
}