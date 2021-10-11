#pragma once
#include "Shader.h"

namespace BeastEngine
{
	class Texture : public Object
	{
	public:
		static std::shared_ptr<Texture> Load(const std::string& texture_path); //テクスチャをファイルを指定し読み込む
		void Set(UINT Slot, Shader::Shader_Type type); //テクスチャをシェーダーにステージする
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Get_Resource() const { return shader_resource_view; }
		[[nodiscard]] UINT Get_Width() const { return texture2d_desc.Width; }	//テクスチャの横幅を返す
		[[nodiscard]] UINT Get_Height() const { return texture2d_desc.Height; } //テクスチャの縦幅を返す
		[[nodiscard]] std::string Get_Path() const { return path; }

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>       sampler = nullptr;
		// テクスチャ情報
		D3D11_TEXTURE2D_DESC texture2d_desc = {};
		std::string path;
	};
}