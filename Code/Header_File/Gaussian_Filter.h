#pragma once
#include "DxSystem.h"
#include "Original_Math.h"

namespace BeastEngine
{
	class Material;

	//ガウシアンフィルタ
	class Gaussian_Filter
	{
	public:
		Gaussian_Filter(const Vector2& size, const DXGI_FORMAT format, const float& dispersion);

		void Create_Render_Target(const Vector2& size, const DXGI_FORMAT format); //レンダーターゲットの生成
		void Set_Weight(const float& dispersion); //フィルタ強度を設定する
		void Filtering_Gaussian(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv); //フィルタリング実行
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Get_Texture() { return target[1].shader_resource_view; } //フィルタリングされたテクスチャを取得する

	private:
		struct Render_Target
		{
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture_render_target;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
		};

		struct Vertex
		{
			Vector3 pos; //位置
			Vector2 tex; //UV座標
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

		Vector2 texture_size;
		Render_Target target[2];
		std::shared_ptr<Material> material[2];
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	};
}