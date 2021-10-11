#pragma once
#include <memory>
#include "DxSystem.h"
#include "Original_Math.h"

namespace BeastEngine
{
	class Material;

	class SkyBox
	{
	public:
		SkyBox();

		void Render(const Vector3& pos);

	private:
		std::shared_ptr<Material> material;

		UINT index_count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer; // 天球頂点バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer; // 天球インデックスバッファ
	};
}