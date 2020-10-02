#pragma once
#include "Light.h"

class Light_Manager
{
public:
	Light_Manager();
	static void Reset();
	static void Add(std::shared_ptr<Light> light);
	static std::list<std::weak_ptr<Light>> Light_list;
private:

	struct CbShadowMap
	{
		Vector3 color;
		float bias;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer_CbShadowMap;		//コンスタントバッファ
};