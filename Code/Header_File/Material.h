#pragma once
#include "DxSystem.h"
#include "Shader.h"
#include <unordered_map>

namespace BeastEngine
{
	class SkinMesh_Renderer;
	class Mesh_Renderer;
	class Sprite_Renderer;
	class SkyBox;
	class Debug_Draw_Manager;
	class Texture;

	class Material : public BeastEngine::Object
	{
	public:
		std::string name;
		std::shared_ptr<BeastEngine::Shader> shader[5];
		std::shared_ptr<BeastEngine::Texture> texture[5];

		BeastEngine::Vector4 color = { 1,1,1,1 };
		int render_queue = 2000;

		struct Shader_Info
		{
			std::string shader_pass;
			std::string shader_name;
			std::string shader_fullpass;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(shader_pass, shader_name, shader_fullpass);
			}
		};
		Shader_Info shader_info[5];

		enum class Texture_Type
		{
			Main,
			Specular,
			Normal,
			Height,
			Emission,
			//追加したらMaterialのテクスチャ配列と情報配列のサイズ,Active_Texture()を書き換えること
		};
		struct Texture_Info
		{
			std::string texture_pass;
			std::string texture_name;
			std::string texture_fullpass;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(texture_pass, texture_name, texture_fullpass);
			}
		};
		Texture_Info texture_info[5];

		static std::shared_ptr<Material> Create(const std::string& Material_Pass, const std::string& Material_Name);
		void Save(const std::string& path = "");
		void Set_Texture(Texture_Type texture_type, const std::string& filepath, const std::string& filename);
		void Set_Shader(const std::string& shader_path, BeastEngine::Shader::Shader_Type shader_type);

	private:
		std::string self_save_pass;

		BS_State blend_state = BS_State::Off;
		RS_State rasterizer_state = RS_State::Cull_Back;
		DS_State depth_stencil_state = DS_State::LEqual;

		static void Initialize(std::shared_ptr<Material>& mat, std::string Material_FullPass);
		void Set_Texture_All();
		void Set_Shader_All();
		void Active_Texture(bool Use_Material = true);
		void Active_Shader();
		void Draw_ImGui();

		friend class BeastEngine::Debug_Draw_Manager;
		friend class BeastEngine::SkyBox;
		friend class BeastEngine::SkinMesh_Renderer;
		friend class BeastEngine::Mesh_Renderer;
		friend class BeastEngine::Sprite_Renderer;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Object>(this), name, color, shader_info, texture_info, blend_state, rasterizer_state, depth_stencil_state, render_queue);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Material)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Material)
CEREAL_CLASS_VERSION(BeastEngine::Material, 1)
CEREAL_CLASS_VERSION(BeastEngine::Material::Shader_Info, 1)
CEREAL_CLASS_VERSION(BeastEngine::Material::Texture_Info, 1)