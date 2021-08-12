#pragma once
#include "Shader.h"

namespace BeastEngine
{
	class Texture : public BeastEngine::Object
	{
	public:
		static std::shared_ptr<BeastEngine::Texture> Load(const std::string& texture_path); //テクスチャをファイルを指定し読み込む
		void Set(UINT Slot, BeastEngine::Shader::Shader_Type type);								 //テクスチャをシェーダーにステージする
		UINT Get_Width() { return texture2d_desc.Width; }						 //テクスチャの横幅を返す
		UINT Get_Height() { return texture2d_desc.Height; }						 //テクスチャの縦幅を返す
		std::string Get_Path() { return path; }

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>       sampler = nullptr;
		// テクスチャ情報
		D3D11_TEXTURE2D_DESC texture2d_desc = {};
		std::string path;
	};
}