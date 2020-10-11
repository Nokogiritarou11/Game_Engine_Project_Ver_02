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

unordered_map<string, shared_ptr<Material>> Material::mat_cache;

shared_ptr<Material> Material::Create(std::string Material_Pass, std::string Material_Name, WCHAR* VS_Name, WCHAR* PS_Name)
{
	auto it = mat_cache.find(Material_Pass + Material_Name + ".mat");
	if (it != mat_cache.end())
	{
		return it->second;
	}
	else
	{
		shared_ptr<Material> mat = make_shared<Material>();
		for (size_t i = 0; i < 5; i++)
		{
			mat->texture[i] = make_shared<Texture>();
		}
		mat->name = Material_Name;
		mat->shader = make_shared<Shader>();
		mat->shader->Create_VS(VS_Name, "VSMain");
		mat->shader->Create_PS(PS_Name, "PSMain");

		{
			char char_pass[MAX_PATH];
			string str_pass;
			WideCharToMultiByte(CP_ACP, 0, VS_Name, -1, char_pass, MAX_PATH, NULL, NULL);
			str_pass = char_pass;
			int path_i = str_pass.find_last_of("\\") + 1;//7
			int ext_i = str_pass.find_last_of(".");//10
			mat->shader_info[VS].Shader_FullPass = str_pass;
			mat->shader_info[VS].Shader_Pass = str_pass.substr(0, path_i); //ファイルまでのディレクトリ
			string filename = str_pass.substr(path_i, ext_i - path_i); //ファイル名
			mat->shader_info[VS].Shader_Name = filename;
		}
		{
			char char_pass[MAX_PATH];
			string str_pass;
			WideCharToMultiByte(CP_ACP, 0, PS_Name, -1, char_pass, MAX_PATH, NULL, NULL);
			str_pass = char_pass;
			int path_i = str_pass.find_last_of("\\") + 1;//7
			int ext_i = str_pass.find_last_of(".");//10
			mat->shader_info[PS].Shader_FullPass = str_pass;
			mat->shader_info[PS].Shader_Pass = str_pass.substr(0, path_i); //ファイルまでのディレクトリ
			string filename = str_pass.substr(path_i, ext_i - path_i); //ファイル名
			mat->shader_info[PS].Shader_Name = filename;
		}
		mat->Self_Save_Pass = Material_Pass + Material_Name + ".mat";
		mat_cache.insert(make_pair(Material_Pass + Material_Name + ".mat", mat));
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
			mat->texture[i] = make_shared<Texture>();
		}
		mat->Set_Texture_All();
		mat->shader = make_shared<Shader>();
		setlocale(LC_ALL, "japanese");
		wchar_t vs[MAX_PATH] = { 0 };
		wchar_t ps[MAX_PATH] = { 0 };
		size_t ret = 0;
		mbstowcs_s(&ret, vs, MAX_PATH, mat->shader_info[VS].Shader_FullPass.c_str(), _TRUNCATE);
		ret = 0;
		mbstowcs_s(&ret, ps, MAX_PATH, mat->shader_info[PS].Shader_FullPass.c_str(), _TRUNCATE);
		mat->shader->Create_VS(vs, "VSMain");
		mat->shader->Create_PS(ps, "PSMain");
		mat->Self_Save_Pass = Material_FullPass;
		mat_cache.insert(make_pair(Material_FullPass, mat));
	}
}

void Material::Set_Texture(int texture_type, std::string filename)
{
	texture[texture_type]->Load(filename);
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

void Material::Draw_ImGui()
{
	ImGui::Text(name.c_str());
	float out_color[4] = { color.x,color.y,color.z,color.w };
	ImGui::ColorEdit3("Color", out_color);
	color = { out_color[0],out_color[1] ,out_color[2] ,out_color[3] };
	if (ImGui::Button(u8"保存"))
	{
		ofstream ss(Self_Save_Pass, ios::binary);
		{
			cereal::BinaryOutputArchive o_archive(ss);
			o_archive(shared_from_this());
		}
	}
}