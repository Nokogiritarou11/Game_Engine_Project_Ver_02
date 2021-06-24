#pragma once
#include "Mesh.h"
#include "Renderer.h"

namespace BeastEngine
{
	class Shader;

	class SkinMesh_Renderer : public BeastEngine::Renderer
	{
	public:

	private:
		std::shared_ptr<BeastEngine::Mesh> mesh_data;

		struct Constant_Buffer_Mesh
		{
#define MAX_BONES 128
			BeastEngine::Matrix bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
		};
		struct Constant_Buffer_Color
		{
			BeastEngine::Vector4	material_color;
		};

		static ComPtr <ID3D11Buffer> constant_buffer_mesh;  //コンスタントバッファ(メッシュデータ)
		static ComPtr <ID3D11Buffer> constant_buffer_color; //コンスタントバッファ(カラー)

		std::vector<Constant_Buffer_Mesh> buffer_mesh;
		std::vector<std::weak_ptr<BeastEngine::Transform>> bones;

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj);
		void Render(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		void Render_Shadow(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		bool Draw_ImGui() override;
		void Set_Active(bool value) override;
		void Recalculate_Frame(BeastEngine::Mesh::mesh& mesh, int index);

		void Set_Mesh(std::shared_ptr<BeastEngine::Mesh> Mesh_Data); //メッシュデータを設定する

		void Reset();

		std::string file_name;
		std::string file_path;

		static std::unique_ptr<BeastEngine::Shader> shadow_shader;
		static std::unique_ptr<BeastEngine::Shader> vertex_shader;

		friend class FBX_Converter;
		friend class Animator;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Renderer>(this), file_name, file_path, bones);
		}
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::SkinMesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::SkinMesh_Renderer)