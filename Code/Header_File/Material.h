#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include "DxSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "Original_Math.h"
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"

class Material : public std::enable_shared_from_this<Material>
{
public:
	std::string name;
	std::unique_ptr<Shader> shader;
	std::unique_ptr<Texture> texture[5];

	Vector4 color = { 1,1,1,1 };

	struct Shader_Info
	{
		std::string Shader_Pass;
		std::string Shader_Name;
		std::string Shader_FullPass;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(Shader_Pass, Shader_Name, Shader_FullPass);
		}
	};
	Shader_Info shader_info[5];
	enum class Shader_Type { VS, PS, GS, HS, DS };

	struct Texture_Info
	{
		std::string Texture_Pass;
		std::string Texture_Name;
		std::string Texture_FullPass;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(Texture_Pass, Texture_Name, Texture_FullPass);
		}
	};
	Texture_Info texture_info[5];

	static std::shared_ptr<Material> Create(const std::string& Material_Pass, const std::string& Material_Name, WCHAR* PS_Name);
	void Save(const std::string& path = "");
	void Set_Texture(Texture::Texture_Type texture_type, const std::string& filepath, const std::string& filename);

private:
	static std::unordered_map<std::string, std::shared_ptr<Material>> mat_cache;

	std::string Self_Save_Pass;

	BS_State BlendState = BS_State::Off;
	RS_State RasterizerState = RS_State::Cull_Back;
	DS_State DepthStencilState = DS_State::LEqual;

	static void Initialize(std::shared_ptr<Material>& mat, std::string Material_FullPass);
	void Set_Texture_All();
	void Active_Texture(bool Use_Material = true);
	void Active_Shader();
	void Draw_ImGui();

	friend class SkyBox;
	friend class SkinMesh_Renderer;
	friend class Mesh_Renderer;
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(name, color, shader_info, texture_info, BlendState, RasterizerState, DepthStencilState);
	}
};