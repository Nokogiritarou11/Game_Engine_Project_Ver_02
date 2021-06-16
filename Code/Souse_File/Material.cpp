#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include <clocale>
#include <unordered_map>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include "Include_ImGui.h"
using namespace std;
using namespace BeastEngine;

unordered_map<string, shared_ptr<Material>> Material::mat_cache;

shared_ptr<Material> Material::Create(const string& Material_Pass, const string& Material_Name, WCHAR* PS_Name)
{
	auto it = mat_cache.find(Material_Pass + Material_Name + ".mat");
	if (it != mat_cache.end())
	{
		return it->second;
	}
	else
	{
		shared_ptr<Material> mat = make_unique<Material>();
		for (size_t i = 0; i < 5; i++)
		{
			mat->texture[i] = make_unique<Texture>();
		}
		mat->name = Material_Name;
		mat->shader = make_unique<Shader>();

		{
			char char_pass[MAX_PATH];
			string str_pass;
			WideCharToMultiByte(CP_ACP, 0, PS_Name, -1, char_pass, MAX_PATH, NULL, NULL);
			str_pass = char_pass;
			int path_i = str_pass.find_last_of("\\") + 1;//7
			int ext_i = str_pass.find_last_of(".");//10
			mat->shader_info[static_cast<int>(Shader_Type::PS)].Shader_FullPass = str_pass;
			mat->shader_info[static_cast<int>(Shader_Type::PS)].Shader_Pass = str_pass.substr(0, path_i); //ファイルまでのディレクトリ
			string filename = str_pass.substr(path_i, ext_i - path_i); //ファイル名
			mat->shader_info[static_cast<int>(Shader_Type::PS)].Shader_Name = filename;
		}
		mat->Self_Save_Pass = Material_Pass + Material_Name + ".mat";
		return mat;
	}
}

void Material::Initialize(shared_ptr<Material>& mat, string Material_FullPass)
{
	auto it = mat_cache.find(Material_FullPass);
	if (it != mat_cache.end())
	{
		mat = it->second;
	}
	else
	{
		for (size_t i = 0; i < 5; i++)
		{
			mat->texture[i] = make_unique<Texture>();
		}
		mat->Set_Texture_All();
		mat->shader = make_unique<Shader>();
		setlocale(LC_ALL, "japanese");
		wchar_t ps[MAX_PATH] = { 0 };
		size_t ret = 0;
		mbstowcs_s(&ret, ps, MAX_PATH, mat->shader_info[static_cast<int>(Shader_Type::PS)].Shader_FullPass.c_str(), _TRUNCATE);
		mat->shader->Create_PS(ps, "PSMain");

		mat->Self_Save_Pass = Material_FullPass;
		mat_cache.insert(make_pair(Material_FullPass, mat));
	}
}

void Material::Set_Texture(Texture::Texture_Type texture_type, const std::string& filepath, const string& filename)
{
	texture_info[static_cast<int>(texture_type)].Texture_Name = filename;
	texture_info[static_cast<int>(texture_type)].Texture_Pass = filepath;
	texture_info[static_cast<int>(texture_type)].Texture_FullPass = filepath + filename;
	texture[static_cast<int>(texture_type)]->Load(texture_info[static_cast<int>(texture_type)].Texture_FullPass);
}

void Material::Set_Texture_All()
{
	for (size_t i = 0; i < 5; i++)
	{
		if (!texture_info[i].Texture_FullPass.empty())
		{
			texture[i]->Load(texture_info[i].Texture_FullPass);
		}
	}
}

void Material::Active_Texture(bool Use_Material)
{
	for (size_t i = 0; i < 5; i++)
	{
		if (texture[i]->Texture_Have)
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
		save_path = Self_Save_Pass;
	}
	else
	{
		save_path = path;
	}
	ofstream ss(save_path, ios::binary);
	{
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(shared_from_this());
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
		depth_current = static_cast<int>(DepthStencilState);
		if (ImGui::Combo("DepthMode", &depth_current, depth, IM_ARRAYSIZE(depth)))
		{
			DepthStencilState = DS_State(depth_current);
		}

		for (int i = 0; i < 3; ++i) ImGui::Spacing();

		static const char* cull[] = { "Back", "Front", "None" };
		static int cull_current = 0;
		cull_current = static_cast<int>(RasterizerState);
		if (ImGui::Combo("Culling", &cull_current, cull, IM_ARRAYSIZE(cull)))
		{
			RasterizerState = RS_State(cull_current);
		}

		for (int i = 0; i < 3; ++i) ImGui::Spacing();

		static const char* blends[] = { "Off", "Alpha", "Alpha_Test", "Transparent", "Add", "Subtract", "Replace", "Multiply" };
		static int blend_current = 0;
		blend_current = static_cast<int>(BlendState);
		if (ImGui::Combo("BlendMode", &blend_current, blends, IM_ARRAYSIZE(blends)))
		{
			BlendState = BS_State(blend_current);
		}

		for (int i = 0; i < 3; ++i) ImGui::Spacing();

		if (ImGui::Button(u8"保存"))
		{
			Save();
		}
		ImGui::TreePop();
	}
}