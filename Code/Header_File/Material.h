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

class Material
{
public:
	std::string name;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture[5];

	Vector4 color = { 1,1,1,1 };
	Vector2 UV_Origin = { 0,0 };
	Vector2 UV_Size = { 0,0 };

	int BlendState = DxSystem::BS_NONE;
	int RasterizerState = DxSystem::RS_CULL_NONE;
	int DepthStencilState = DxSystem::DS_TRUE;

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
	enum Shader_Type { VS, PS, GS, HS, DS };

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

	static std::shared_ptr<Material> Create(std::string Material_Pass, std::string Material_Name, WCHAR* VS_Name, WCHAR* PS_Name);
	static void Initialize(shared_ptr<Material>& mat, std::string Material_FullPass);
	void Set_Texture(int texture_type, std::string filename);
	void Set_Texture_All();
	void Active_Texture(bool Use_Material = true);
	void Active_Shader();
	void Draw_ImGui();

private:
	static std::unordered_map<std::string, std::shared_ptr<Material>> mat_cache;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(name, color, shader_info, texture_info);
	}
};