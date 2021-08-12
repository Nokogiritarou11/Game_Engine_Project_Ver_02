#pragma once
#include <vector>
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

		void Render(const BeastEngine::Vector3& pos);

	private:
		std::shared_ptr<BeastEngine::Material> material;

		UINT index_count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer; // �V�����_�o�b�t�@
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer; // �V���C���f�b�N�X�o�b�t�@
	};
}