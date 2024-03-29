#pragma once
#include "Renderer.h"

namespace BeastEngine
{
	class Shader;
	class Compute_Shader;
	class Mesh;

	//スキンメッシュレンダラーコンポーネント
	class SkinMesh_Renderer : public Renderer
	{
	private:
		std::shared_ptr<Mesh> mesh;

		//スキンメッシュ用コンスタントバッファ
		struct Constant_Buffer_Mesh
		{
#define MAX_BONES 128
			Matrix bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
		};
		//カラーコンスタントバッファ
		struct Constant_Buffer_Color
		{
			Vector4 material_color;
		};

		static Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_mesh;

		Constant_Buffer_Mesh buffer_mesh;
		std::vector<std::weak_ptr<Transform>> bones;

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		void Render(int subset_number = 0) override;                      //描画実行
		void Render_Shadow(int subset_number = 0) override;               //シャドウマップに描画実行
		bool Draw_ImGui() override;                                       //ImGui描画
		void Set_Active(bool value) override;                             //アクティブ状態を設定する
		void Recalculate_Frame() override;                                //ボーン行列をコンピュートシェーダーで計算する

		void Set_Mesh(const std::shared_ptr<Mesh>& mesh_data); //メッシュデータを設定する

		void Reset(); //リセット

		std::string file_path;
		std::shared_ptr<Compute_Shader> compute_shader;

		friend class Fbx_Converter;
		friend class Animator;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Renderer>(this), file_path, bones);
		}
	};
}

REGISTER_COMPONENT(SkinMesh_Renderer)
CEREAL_REGISTER_TYPE(BeastEngine::SkinMesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::SkinMesh_Renderer)
CEREAL_CLASS_VERSION(BeastEngine::SkinMesh_Renderer, 1)