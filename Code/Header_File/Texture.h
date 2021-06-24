#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <wrl.h>
#include <string>
#include <clocale>
#include <tchar.h>
using Microsoft::WRL::ComPtr;

namespace BeastEngine
{
	class Texture
	{
	protected:
		ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
		ComPtr<ID3D11SamplerState>       sampler = nullptr;
		ComPtr<ID3D11RenderTargetView>   render_target_view = nullptr;

		// テクスチャ情報
		D3D11_TEXTURE2D_DESC texture2d_desc;


	public:
		Texture();
		virtual ~Texture();

		bool load(std::string filename, int sampler_state = 0);							//テクスチャをファイルを指定し読み込む(失敗したらFalseを返す)
		void Set(UINT Slot = 0, BOOL flg = TRUE);										//テクスチャをシェーダーにステージする
		UINT Get_Width() { return texture2d_desc.Width; }								//テクスチャの横幅を返す
		UINT Get_Height() { return texture2d_desc.Height; }								//テクスチャの縦幅を返す
		ID3D11RenderTargetView* Get_Render_Target() { return render_target_view.Get(); }	//レンダーターゲットを返す

		bool has_texture = false;														//テクスチャデータを保持しているか

		enum class Texture_Type
		{
			Main,
			Specular,
			Normal,
			Height,
			Emission,
			//追加したらMaterialのテクスチャ配列と情報配列のサイズ,Active_Texture()を書き換えること
		};
	};
}