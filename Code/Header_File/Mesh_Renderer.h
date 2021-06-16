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
		std::shared_ptr<BeastEngine::Mesh> mesh_data;

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		void Render(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		void Render_Shadow(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		bool Draw_ImGui() override;
		void SetActive(bool value) override;
		void Recalculate_Buffer();

		struct CbMesh
		{
			BeastEngine::Matrix world = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		};
		struct CbColor
		{
			BeastEngine::Vector4	materialColor;
		};

		CbMesh mesh_buffer;

		static ComPtr <ID3D11Buffer> ConstantBuffer_CbMesh;  //コンスタントバッファ(メッシュデータ)
		static ComPtr <ID3D11Buffer> ConstantBuffer_CbColor; //コンスタントバッファ(カラー)

		void Reset();

		std::string file_name;
		std::string file_path;

		static std::unique_ptr<BeastEngine::Shader> shadow_shader;
		static std::unique_ptr<BeastEngine::Shader> vertex_shader;

		friend class FBX_Converter;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Renderer>(this), file_name, file_path);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Mesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Mesh_Renderer)