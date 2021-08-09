#include "Scene_Constants.hlsli"

VS_OUT main(
	float3 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD
)
{
	VS_OUT vout;

	float4 pos = float4(position, 1.0f);
	float4 nor = float4(normal, 0.0f);
	float4 tan = float4(tangent, 0.0f);

	vout.position = mul(viewProjection, pos);
	vout.normal = nor;
	vout.tangent = tan;
	vout.texcoord = texcoord;
	vout.sdwcoord = mul(shadowMatrix, pos + vout.normal * bias);

	return vout;
}