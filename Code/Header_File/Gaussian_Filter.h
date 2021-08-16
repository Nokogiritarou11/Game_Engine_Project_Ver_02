#pragma once
#include "DxSystem.h"
#include "Original_Math.h"
#include <vector>

namespace BeastEngine
{
	class Material;

	class Gaussian_Filter
	{
	public:
		Gaussian_Filter(const BeastEngine::Vector2& size, const DXGI_FORMAT format, const float& dispersion);
		void Create_Render_Target(const BeastEngine::Vector2& size, const DXGI_FORMAT format);
		void Set_Weight(const float& dispersion);
		void Filtering_Gaussian(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv);
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Get_Texture() { return target[1].shader_resource_view; }

	private:
		struct Render_Target
		{
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture_render_target;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
		};

		struct Vertex
		{
			BeastEngine::Vector3 pos; //�ʒu
			BeastEngine::Vector2 tex; //UV���W
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

		BeastEngine::Vector2 texture_size;
		Render_Target target[2];
		std::shared_ptr<BeastEngine::Material> material[2];
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	};
}