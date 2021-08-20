#pragma once
#include "DxSystem.h"
#include "Shader.h"
#include <unordered_map>
#include <cstddef>
#include "cereal/types/unordered_map.hpp"

namespace BeastEngine
{
	class SkinMesh_Renderer;
	class Mesh_Renderer;
	class Sprite_Renderer;
	class Gaussian_Filter;
	class SkyBox;
	class Render_Manager;
	class Shadow_Manager;
	class Debug_Draw_Manager;
	class Texture;

	class Material : public BeastEngine::Object
	{
	public:
		int render_queue = 2000;

		static std::shared_ptr<Material> Create(const std::string& vertex_path, const std::string& pixel_path = "", const std::string& geometry_path = "");
		static std::shared_ptr<Material> Load_Material(const std::string& fullpath);

		void Save(const std::string& path = "");

		void Set_Shader(const std::string& path, BeastEngine::Shader::Shader_Type shader_type);
		void Set_Blend_State(BS_State state);
		void Set_Rasterizer_State(RS_State state);
		void Set_Depth_Stencil_State(DS_State state);

		void Set_Texture(const std::string& texture_name, const std::shared_ptr<BeastEngine::Texture>& texture);
		void Set_Int(const std::string& int_name, int& value);
		void Set_Float(const std::string& float_name, float& value);
		void Set_Vector2(const std::string& vector_name, BeastEngine::Vector2& value);
		void Set_Vector3(const std::string& vector_name, BeastEngine::Vector3& value);
		void Set_Vector4(const std::string& vector_name, BeastEngine::Vector4& value);
		void Set_Matrix(const std::string& matrix_name, BeastEngine::Matrix& value);

		std::shared_ptr<BeastEngine::Texture> Get_Texture(const std::string& texture_name);
		int Get_Int(const std::string& int_name);
		float Get_Float(const std::string& float_name);
		BeastEngine::Vector2 Get_Vector2(const std::string& vector_name);
		BeastEngine::Vector3 Get_Vector3(const std::string& vector_name);
		BeastEngine::Vector4 Get_Vector4(const std::string& vector_name);
		BeastEngine::Matrix Get_Matrix(const std::string& matrix_name);

	private:
		struct Shader_Info
		{
			std::string shader_path;
			std::shared_ptr<BeastEngine::Shader> shader;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(shader_path);
			}
		};

		struct ConstantBuffer_Info
		{
			UINT register_number = 0;
			std::vector<Shader::Shader_Type> staging_shader;
			std::vector<std::byte> byte_data;
			Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(register_number, staging_shader, byte_data);
			}
		};

		struct Parameter_Info
		{
			Shader::Parameter_Type type;
			std::string parent_name;
			UINT size = 0;
			UINT offset = 0;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(type, parent_name, size, offset);
			}
		};

		struct Texture_Info
		{
			UINT register_number = 0;
			std::vector<Shader::Shader_Type> staging_shader;
			std::string texture_path;
			std::shared_ptr<BeastEngine::Texture> texture;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(register_number, staging_shader, texture_path);
			}
		};

		std::string self_save_pass;
		Shader_Info shader_info[5];

		std::unordered_map<std::string, ConstantBuffer_Info> constant_buffer_info;
		std::unordered_map<std::string, Parameter_Info> parameter_info;
		std::unordered_map<std::string, Texture_Info> texture_info;

		static BS_State binding_blend_state;
		static RS_State binding_rasterizer_state;
		static DS_State binding_depth_stencil_State;

		BS_State blend_state = BS_State::Off;
		RS_State rasterizer_state = RS_State::Cull_Back;
		DS_State depth_stencil_state = DS_State::GEqual;

		void Reflect_Shader();
		void Reflect_Texture();
		void Create_ConstantBuffer(ConstantBuffer_Info& info, const UINT& size);
		void Set_Parameter(const std::string& parameter_name, void* value, const Shader::Parameter_Type& type);
		void Initialize_Texture();
		void Initialize_Shader();
		void Active();

		void Draw_ImGui();

		friend class BeastEngine::Render_Manager;
		friend class BeastEngine::Shadow_Manager;
		friend class BeastEngine::Debug_Draw_Manager;
		friend class BeastEngine::Gaussian_Filter;
		friend class BeastEngine::SkyBox;
		friend class BeastEngine::SkinMesh_Renderer;
		friend class BeastEngine::Mesh_Renderer;
		friend class BeastEngine::Sprite_Renderer;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Object>(this), self_save_pass, shader_info, constant_buffer_info, parameter_info, texture_info, render_queue, blend_state, rasterizer_state, depth_stencil_state);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Material)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Material)
CEREAL_CLASS_VERSION(BeastEngine::Material, 1)