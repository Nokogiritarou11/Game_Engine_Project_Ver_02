#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <wrl.h>
#include "Renderer.h"

namespace BeastEngine
{
	class Texture;
	class Material;

	//スプライトレンダラーコンポーネント
	class Sprite_Renderer : public Renderer
	{
	public:
		Vector2 size = { 100,100 };		    //表示サイズ
		Vector2 uv_origin = { 0,0 };		//テクスチャのUV原点
		Vector2 uv_size = { 100,100 };		//テクスチャから切り出すサイズ(UV原点基準)
		Vector4 color = { 1,1,1,1 };	    //テクスチャカラー({1,1,1,1}でオリジナル)
		std::shared_ptr<Texture> texture;	//描画するテクスチャ

	private:
		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		void Recalculate_Frame() override; //表示座標を計算する
		void Render(int subset_number = 0) override; //描画実行
		bool Draw_ImGui() override; //ImGui描画
		void Set_Active(bool value) override; //アクティブ状態を設定する

		struct Vertex
		{
			Vector3 pos;	//位置
			Vector2 tex;	//UV座標
			Vector4 color;	//頂点色
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

		std::string file_name;
		std::string file_path;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Renderer>(this), size, uv_origin, uv_size, color, file_name, file_path);
		}
	};
}

REGISTER_COMPONENT(Sprite_Renderer)
CEREAL_REGISTER_TYPE(BeastEngine::Sprite_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Sprite_Renderer)
CEREAL_CLASS_VERSION(BeastEngine::Sprite_Renderer, 1)