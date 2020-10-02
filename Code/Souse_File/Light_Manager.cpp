#include "Light_Manager.h"
using namespace std;

list<weak_ptr<Light>> Light_Manager::Light_list;

Light_Manager::Light_Manager()
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
		assert(SUCCEEDED(hr));
	}
}

void Light_Manager::Reset()
{
	Light_list.clear();
}

void Light_Manager::Add(std::shared_ptr<Light> light)
{
	Light_list.emplace_back(light);
}