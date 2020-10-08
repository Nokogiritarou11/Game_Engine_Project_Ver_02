#include "Shader\\Standard_Shader.hlsli"
#include "Shader\\Scene_Constants.hlsli"

Texture2D shadowMap : register(t0);
Texture2D diffuseMap : register(t1);

SamplerComparisonState  ShadowMapSamplerState   : register(s0);
SamplerState diffuseMapSamplerState : register(s1);


float4 PSMain(VS_OUT pin) : SV_TARGET
{
	float4 outcolor;

	float4 mapdiff = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * (pin.color * float4(0.4f, 0.4f, 0.4f, 1.0f) + float4(0.6f, 0.6f, 0.6f,0.0f));

	// �V���h�E�}�b�v�̐[�x�l�Ɣ�r����.
	float3 shadowCoord = pin.sdwcoord.xyz / pin.sdwcoord.w;
	// �ő�[�x�X�΂����߂�.
	float  maxDepthSlope = max(abs(ddx(shadowCoord.z)), abs(ddy(shadowCoord.z)));
	float  shadowThreshold = 1.0f;      // �V���h�E�ɂ��邩�ǂ�����臒l�ł�.
	float  slopeScaledBias = 0.0001f;     // �[�x�X��.
	float  depthBiasClamp = 0.01f;      // �o�C�A�X�N�����v�l.
	float3 shadowColor = float3(0.65f, 0.65f, 0.65f);

	float  shadowBias = bias + slopeScaledBias * maxDepthSlope;
	shadowBias = min(shadowBias, depthBiasClamp);
	shadowThreshold = shadowMap.SampleCmpLevelZero(ShadowMapSamplerState, shadowCoord.xy, shadowCoord.z - shadowBias);
	shadowColor = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold);

	outcolor.rgb = mapdiff.rgb * shadowColor;
	outcolor.a = mapdiff.a;

	return outcolor;
}

