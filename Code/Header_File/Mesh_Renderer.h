#pragma once
#include "Renderer.h"

namespace BeastEngine
{
	class Mesh;
	class Material;
	class Compute_Shader;

	//メッシュレンダラーコンポーネント
	class Mesh_Renderer : public Renderer
	{
	public:
		void Set_Mesh(const std::shared_ptr<Mesh>& mesh_data); //メッシュデータを設定する

	private:
		std::shared_ptr<Mesh> mesh;

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		void Render(int subset_number = 0) override; //描画実行
		void Render_Shadow(int subset_number = 0) override; //シャドウマップへ描画実行
		bool Draw_ImGui() override; //ImGui描画
		void Set_Active(bool value) override; //アクティブ状態を設定する
		void Recalculate_Frame() override; //コンピュートシェーダーで頂点を計算する

		//ワールド変換用コンスタントバッファ
		struct Constant_Buffer_Mesh
		{
			Matrix world = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		};
		//カラー情報コンスタントバッファ
		struct Constant_Buffer_Color
		{
			Vector4	material_color;
		};

		Constant_Buffer_Mesh buffer_mesh;
		static Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_mesh;

		void Reset(); //データのクリア

		std::string file_path;
		std::shared_ptr<Compute_Shader> compute_shader;

		friend class Fbx_Converter;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Renderer>(this), file_path);
		}
	};
}

REGISTER_COMPONENT(Mesh_Renderer)
CEREAL_REGISTER_TYPE(BeastEngine::Mesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Mesh_Renderer)
CEREAL_CLASS_VERSION(BeastEngine::Mesh_Renderer, 1)