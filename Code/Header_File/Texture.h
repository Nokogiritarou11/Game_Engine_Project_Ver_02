#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <unordered_map>
#include "Object.h"

namespace BeastEngine
{
	class Texture : public BeastEngine::Object
	{
	public:
		static std::shared_ptr<BeastEngine::Texture> Load(std::string filename);		//テクスチャをファイルを指定し読み込む(失敗したらFalseを返す)
		void Set(UINT Slot = 0, BOOL flg = TRUE);										//テクスチャをシェーダーにステージする
		UINT Get_Width() { return texture2d_desc.Width; }								//テクスチャの横幅を返す
		UINT Get_Height() { return texture2d_desc.Height; }								//テクスチャの縦幅を返す
		ID3D11RenderTargetView* Get_Render_Target() { return render_target_view.Get(); }//レンダーターゲットを返す												//テクスチャデータを保持しているか

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>       sampler = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   render_target_view = nullptr;

		// テクスチャ情報
		D3D11_TEXTURE2D_DESC texture2d_desc;
	};
}