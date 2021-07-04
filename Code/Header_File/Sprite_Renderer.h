#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
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

		BeastEngine::Vector2 size = { 100,100 };			//表示サイズ
		BeastEngine::Vector2 uv_origin = { 0,0 };		//テクスチャのUV原点
		BeastEngine::Vector2 uv_size = { 100,100 };		//テクスチャから切り出すサイズ(UV原点基準)
		BeastEngine::Vector4 color = { 1,1,1,1 };		//テクスチャカラー({1,1,1,1}でオリジナル)
		std::shared_ptr<BeastEngine::Texture> texture;	//描画するテクスチャ

	private:

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj);
		void Render(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		bool Draw_ImGui() override;
		void Set_Active(bool value) override;

		struct Vertex
		{
			BeastEngine::Vector3 pos;	//位置
			BeastEngine::Vector2 tex;	//UV座標
			BeastEngine::Vector4 color;	//頂点色
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

		std::string file_name;
		std::string file_path;

		static std::shared_ptr<BeastEngine::Shader> default_shader;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Renderer>(this), size, uv_origin, uv_size, color, file_name, file_path);
		}
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Sprite_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Sprite_Renderer)