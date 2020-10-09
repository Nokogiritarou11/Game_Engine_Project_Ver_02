#include "Shader\\SkyBox_Shader.hlsli"
#include "Shader\\Scene_Constants.hlsli"

TextureCube cubemap : register(t1);
SamplerState diffuseMapSamplerState : register(s1);

float4 PSMain(VS_OUT pin) : SV_TARGET
{
	//float luminance = max(1.0, dot(colour.rgb, float3(0.2126, 0.7152, 0.0722))) * directional_light.colour.w;
	//float3 light_colour = normalize(colour.rgb) * luminance;

	return float4(cubemap.Sample(diffuseMapSamplerState, pin.texcoord).rgb * color.rgb * 1.3f, color.a);
}
