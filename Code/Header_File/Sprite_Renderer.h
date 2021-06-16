#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <string>
#include <wrl.h>
#include "Renderer.h"

namespace BeastEngine
{
	class Shader;
	class Texture;

	class Sprite_Renderer : public BeastEngine::Renderer
	{
	public:

		BeastEngine::Vector2 Size = { 100,100 };			//表示サイズ
		BeastEngine::Vector2 UV_Origin = { 0,0 };		//テクスチャのUV原点
		BeastEngine::Vector2 UV_Size = { 100,100 };		//テクスチャから切り出すサイズ(UV原点基準)
		BeastEngine::Vector4 Color = { 1,1,1,1 };		//テクスチャカラー({1,1,1,1}でオリジナル)
		std::shared_ptr<BeastEngine::Texture> texture;	//描画するテクスチャ

	private:

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj);
		void Render(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		bool Draw_ImGui() override;
		void SetActive(bool value) override;

		struct VERTEX
		{
			BeastEngine::Vector3 Pos;	//位置
			BeastEngine::Vector2 Tex;	//UV座標
			BeastEngine::Vector4 Color;	//頂点色
		};

		ComPtr<ID3D11Buffer> VertexBuffer;

		std::string file_name;
		std::string file_path;

		static std::unique_ptr<BeastEngine::Shader> default_shader;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Renderer>(this), Size, UV_Origin, UV_Size, Color, file_name, file_path);
		}
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Sprite_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Sprite_Renderer)