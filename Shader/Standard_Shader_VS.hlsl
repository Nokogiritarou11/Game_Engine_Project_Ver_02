#include "Shader\\Standard_Shader.hlsli"
#include "Shader\\Scene_Constants.hlsli"

VS_OUT VSMain(
	float3 position : POSITION,
	float3 normal : NORMAL,
	float2 texcoord : TEXCOORD,
	float4 bone_weights : WEIGHTS,
	uint4  bone_indices : BONES
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

	float4 w_pos = float4(p, 1.0f);
	vout.position = mul(w_pos, viewProjection);

	float3 N = normalize(n);
	float3 L = normalize(-lightDirection.xyz);
	float  d = dot(L, N);

	//vout.color.xyz = materialColor.xyz * max(0.75f, d);
	vout.color.xyz = materialColor.xyz * d;
	vout.color.w = materialColor.w;
	vout.texcoord = texcoord;

	///*
	vout.sdwcoord = mul(w_pos, shadowMatrix);
	//*/
	/*
	w_pos = mul(viewProjection, w_pos);
	w_pos /= w_pos.w;
	// テクスチャ座標系
	w_pos.y = -w_pos.y;
	w_pos.xy = 0.5f * w_pos.xy + 0.5f;
	vout.sdwcoord = float4(w_pos.x, w_pos.y, w_pos.z, 0);
	*/
	return vout;
}