#include <clocale>
#include <unordered_map>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Asset_Manager.h"
using namespace std;
using namespace BeastEngine;

shared_ptr<Material> Material::Create(const string& Material_Pass, const string& Material_Name, WCHAR* PS_Name)
{
	auto it = Engine::asset_manager->cache_material.find(Material_Pass + Material_Name + ".mat");
	if (it != Engine::asset_manager->cache_material.end())
	{
		return it->second;
	}
	else
	{
		shared_ptr<Material> mat = make_shared<Material>();
		mat->name = Material_Name;
		{
			char char_pass[MAX_PATH];
			string str_pass;
			WideCharToMultiByte(CP_ACP, 0, PS_Name, -1, char_pass, MAX_PATH, NULL, NULL);
			str_pass = char_pass;
			int path_i = str_pass.find_last_of("\\") + 1;//7
			int ext_i = str_pass.find_last_of(".");//10
			mat->shader_info[static_cast<int>(Shader_Type::PS)].shader_fullpass = str_pass;
			mat->shader_info[static_cast<int>(Shader_Type::PS)].shader_pass = str_pass.substr(0, path_i); //ファイルまでのディレクトリ
			string filename = str_pass.substr(path_i, ext_i - path_i); //ファイル名
			mat->shader_info[static_cast<int>(Shader_Type::PS)].shader_name = filename;
		}
		mat->self_save_pass = Material_Pass + Material_Name + ".mat";
		return mat;
	}
}

void Material::Initialize(shared_ptr<Material>& mat, string Material_FullPass)
{
	auto it = Engine::asset_manager->cache_material.find(Material_FullPass);
	if (it != Engine::asset_manager->cache_material.end())
	{
		mat = it->second;
	}
	else
	{
		mat->Set_Texture_All();
		mat->shader = Shader::Create("", mat->shader_info[static_cast<int>(Shader_Type::PS)].shader_fullpass);

		mat->self_save_pass = Material_FullPass;
		Engine::asset_manager->cache_material.insert(make_pair(Material_FullPass, mat));
	}
}

void Material::Set_Texture(Texture_Type texture_type, const std::string& filepath, const string& filename)
{
	texture_info[static_cast<int>(texture_type)].texture_name = filename;
	texture_info[static_cast<int>(texture_type)].texture_pass = filepath;
	texture_info[static_cast<int>(texture_type)].texture_fullpass = filepath + filename;
	texture[static_cast<int>(texture_type)] = Texture::Load(texture_info[static_cast<int>(texture_type)].texture_fullpass);
}

void Material::Set_Texture_All()
{
	for (size_t i = 0; i < 5; i++)
	{
		if (!texture_info[i].texture_fullpass.empty())
		{
			texture[i] = Texture::Load(texture_info[i].texture_fullpass);
		}
	}
}

void Material::Active_Texture(bool Use_Material)
{
	for (size_t i = 0; i < 5; i++)
	{
		if (texture[i])
		{
			texture[i]->Set(i + 1, Use_Material);
		}
	}
}

void Material::Active_Shader()
{
	shader->Activate();
}

void Material::Save(const string& path)
{
	string save_path;
	if (path.empty())
	{
		save_path = self_save_pass;
	}
	else
	{
		save_path = path;
	}
	ofstream ss(save_path, ios::binary);
	{
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(static_pointer_cast<Material>(shared_from_this()));
	}
}

void Material::Draw_ImGui()
{
	if (ImGui::TreeNode(name.c_str()))
	{
		float out_color[4] = { color.x,color.y,color.z,color.w };
		ImGui::ColorEdit4("Color", out_color);
		color = { out_color[0],out_color[1] ,out_color[2] ,out_color[3] };

		for (int i = 0; i < 3; ++i) ImGui::Spacing();

		static const char* depth[] = { "None", "Less", "Greater", "LEqual", "GEqual", "Equal", "NotEqual", "Always", "None_No_Write", "Less_No_Write", "Greater_No_Write", "LEqual_No_Write", "GEqual_No_Write", "Equal_No_Write", "NotEqual_No_Write", "Always_No_Write" };
		static int depth_current = 0;
		depth_current = static_cast<int>(depth_stencil_state);
		if (ImGui::Combo("DepthMode", &depth_current, depth, IM_ARRAYSIZE(depth)))
		{
			depth_stencil_state = DS_State(depth_current);
		}

		for (int i = 0; i < 3; ++i) ImGui::Spacing();

		static const char* cull[] = { "Back", "Front", "None" };
		static int cull_current = 0;
		cull_current = static_cast<int>(rasterizer_state);
		if (ImGui::Combo("Culling", &cull_current, cull, IM_ARRAYSIZE(cull)))
		{
			rasterizer_state = RS_State(cull_current);
		}

		for (int i = 0; i < 3; ++i) ImGui::Spacing();

		static const char* blends[] = { "Off", "Alpha", "Alpha_Test", "Transparent", "Add", "Subtract", "Replace", "Multiply" };
		static int blend_current = 0;
		blend_current = static_cast<int>(blend_state);
		if (ImGui::Combo("BlendMode", &blend_current, blends, IM_ARRAYSIZE(blends)))
		{
			blend_state = BS_State(blend_current);
		}

		for (int i = 0; i < 3; ++i) ImGui::Spacing();

		if (ImGui::Button(u8"保存"))
		{
			Save();
		}
		ImGui::TreePop();
	}
}