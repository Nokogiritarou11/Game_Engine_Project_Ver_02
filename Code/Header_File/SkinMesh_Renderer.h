#pragma once
#include "Renderer.h"

namespace BeastEngine
{
	class Shader;
	class Compute_Shader;
	class Mesh;

	class SkinMesh_Renderer : public BeastEngine::Renderer
	{
	public:

	private:
		std::shared_ptr<BeastEngine::Mesh> mesh;

		struct Constant_Buffer_Mesh
		{
#define MAX_BONES 128
			BeastEngine::Matrix bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
		};
		struct Constant_Buffer_Color
		{
			BeastEngine::Vector4 material_color;
		};

		static Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_mesh;  //コンスタントバッファ(メッシュデータ)

		Constant_Buffer_Mesh buffer_mesh;
		std::vector<std::weak_ptr<BeastEngine::Transform>> bones;

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		void Render(int subset_number = 0) override;
		void Render_Shadow(int subset_number = 0) override;
		bool Draw_ImGui() override;
		void Set_Active(bool value) override;
		void Recalculate_Frame() override;

		void Set_Mesh(std::shared_ptr<BeastEngine::Mesh> Mesh_Data); //メッシュデータを設定する

		void Reset();

		std::string file_path;
		std::shared_ptr<BeastEngine::Compute_Shader> compute_shader;

		friend class FBX_Converter;
		friend class Animator;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Renderer>(this), file_path, bones);
		}
	};
}

REGISTER_COMPONENT(SkinMesh_Renderer)
CEREAL_REGISTER_TYPE(BeastEngine::SkinMesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::SkinMesh_Renderer)
CEREAL_CLASS_VERSION(BeastEngine::SkinMesh_Renderer, 1)