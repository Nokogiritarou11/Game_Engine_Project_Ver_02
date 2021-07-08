#pragma once
#include "DxSystem.h"
#include "Object.h"
#include <unordered_map>

namespace BeastEngine
{
	class SkinMesh_Renderer;
	class Mesh_Renderer;
	class SkyBox;
	class Shader;
	class Texture;

	class Material : public BeastEngine::Object
	{
	public:
		std::string name;
		std::shared_ptr<BeastEngine::Shader> shader;
		std::shared_ptr<BeastEngine::Texture> texture[5];

		BeastEngine::Vector4 color = { 1,1,1,1 };

		struct Shader_Info
		{
			std::string shader_pass;
			std::string shader_name;
			std::string shader_fullpass;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(shader_pass, shader_name, shader_fullpass);
			}
		};
		Shader_Info shader_info[5];
		enum class Shader_Type { VS, PS, GS, HS, DS };

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
			void serialize(Archive& archive)
			{
				archive(texture_pass, texture_name, texture_fullpass);
			}
		};
		Texture_Info texture_info[5];

		static std::shared_ptr<Material> Create(const std::string& Material_Pass, const std::string& Material_Name, WCHAR* PS_Name);
		void Save(const std::string& path = "");
		void Set_Texture(Texture_Type texture_type, const std::string& filepath, const std::string& filename);

	private:
		std::string self_save_pass;

		BS_State blend_state = BS_State::Off;
		RS_State rasterizer_state = RS_State::Cull_Back;
		DS_State depth_stencil_state = DS_State::LEqual;

		static void Initialize(std::shared_ptr<Material>& mat, std::string Material_FullPass);
		void Set_Texture_All();
		void Active_Texture(bool Use_Material = true);
		void Active_Shader();
		void Draw_ImGui();

		friend class BeastEngine::SkyBox;
		friend class BeastEngine::SkinMesh_Renderer;
		friend class BeastEngine::Mesh_Renderer;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Object>(this), name, color, shader_info, texture_info, blend_state, rasterizer_state, depth_stencil_state);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Material)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Material)