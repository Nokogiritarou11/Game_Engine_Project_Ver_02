#include "Shader\\SkyBox_Shader.hlsli"
#include "Shader\\Scene_Constants.hlsli"

VS_OUT VSMain(VS_IN vin)
{
	VS_OUT vout;

	vout.position = mul(float4(vin.position, 1.0f), mul(world, viewProjection));
	vout.texcoord = normalize(vin.position.xyz);

	return vout;
}