#include "Light.h"
#include "GameObject.h"
#include "Transform.h"
#include "Include_ImGui.h"
#include "Debug_UI.h"
using namespace std;

Light::Light()
{
}
Light::~Light()
{
}

void Light::Initialize()
{
	// 定数バッファの生成
	if (!ConstantBuffer_CbShadowMap)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CbShadowMap);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, nullptr, ConstantBuffer_CbShadowMap.GetAddressOf());
		assert(SUCCEEDED(hr), hr_trace(hr));
	}
}
void Light::Initialize(std::shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;

	shader = make_unique<Shader>();
	shader->Create_VS(L"Shader\\ShadoeoMap_Shader_VS.hlsl", "VSMain");
	shader->Create_PS(L"Shader\\ShadoeoMap_Shader_PS.hlsl", "PSMain");

	// 定数バッファの生成
	if (!ConstantBuffer_CbShadowMap)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CbShadowMap);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, nullptr, ConstantBuffer_CbShadowMap.GetAddressOf());
		assert(SUCCEEDED(hr), hr_trace(hr));
	}
}

void Light::Clear()
{

}

bool Light::Draw_ImGui()
{
	return true;
}