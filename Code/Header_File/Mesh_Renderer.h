#pragma once
#include "Renderer.h"

namespace BeastEngine
{
	class Mesh;
	class Shader;

	class Mesh_Renderer : public BeastEngine::Renderer
	{
	public:
		void Set_Mesh(std::shared_ptr<BeastEngine::Mesh> Mesh_Data); //メッシュデータを設定する

	private:
		std::shared_ptr<BeastEngine::Mesh> mesh;

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		void Render(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		void Render_Shadow(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		bool Draw_ImGui() override;
		void Set_Active(bool value) override;
		void Recalculate_Frame();

		struct Constant_Buffer_Mesh
		{
			BeastEngine::Matrix world = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		};
		struct Constant_Buffer_Color
		{
			BeastEngine::Vector4	material_color;
		};

		Constant_Buffer_Mesh buffer_mesh;

		static Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_mesh;  //コンスタントバッファ(メッシュデータ)
		static Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_color; //コンスタントバッファ(カラー)

		void Reset();

		std::string file_name;
		std::string file_path;

		static std::shared_ptr<BeastEngine::Shader> shadow_shader;
		static std::shared_ptr<BeastEngine::Shader> vertex_shader;

		friend class FBX_Converter;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Renderer>(this), file_name, file_path);
		}
	};
}

REGISTER_COMPONENT(Mesh_Renderer)
CEREAL_REGISTER_TYPE(BeastEngine::Mesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Mesh_Renderer)
CEREAL_CLASS_VERSION(BeastEngine::Mesh_Renderer, 1)