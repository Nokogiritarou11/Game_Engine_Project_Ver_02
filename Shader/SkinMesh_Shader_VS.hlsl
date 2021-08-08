#include "Scene_Constants.hlsli"

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	float4x4 boneTransforms[MAX_BONES];
};

VS_OUT VSMain(
	float3 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 bone_weights : WEIGHTS,
	uint4 bone_indices : BONES
)
{
	VS_OUT vout;

	float4 pos = float4(position, 1.0f);
	float4 nor = float4(normal, 0.0f);
	float4 tan = float4(tangent, 0.0f);
	float3 p = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	float3 t = { 0, 0, 0 };

	for (int i = 0; i < 4; i++)
	{
		p += (bone_weights[i] * mul(boneTransforms[bone_indices[i]], pos)).xyz;
		n += (bone_weights[i] * mul(boneTransforms[bone_indices[i]], nor)).xyz;
		t += (bone_weights[i] * mul(boneTransforms[bone_indices[i]], tan)).xyz;
	}

	float4 w_pos = float4(p, 1.0f);
	vout.position = mul(viewProjection, w_pos);

	vout.normal = float4(normalize(n), 0.0f);
	vout.tangent = float4(normalize(t), 0.0f);
	/*
	float3 N = normalize(n);
	float3 L = normalize(-lightDirection.xyz);
	float  d = dot(L, N);

	//vout.color.xyz = materialColor.xyz * max(0.75f, d);
	vout.color.xyz = materialColor.xyz * d;
	vout.color.w = materialColor.w;
	*/
	vout.texcoord = texcoord;

	///*
	vout.sdwcoord = mul(shadowMatrix, w_pos + vout.normal * bias);
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