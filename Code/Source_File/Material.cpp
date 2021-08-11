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
#include "System_Function.h"
using namespace std;
using namespace BeastEngine;

shared_ptr<Material> Material::Create(const std::string& vertex_path, const std::string& pixel_path, const std::string& geometry_path)
{
	shared_ptr<Material> mat = make_shared<Material>();
	mat->name = "material";
	mat->Set_Shader(vertex_path, Shader::Shader_Type::Vertex);
	mat->Set_Shader(pixel_path, Shader::Shader_Type::Pixel);
	mat->Set_Shader(geometry_path, Shader::Shader_Type::Geometry);
	Engine::asset_manager->Registration_Asset(mat);
	return mat;
}

shared_ptr<Material> Material::Load_Material(const string& fullpath)
{
	auto it = Engine::asset_manager->cache_material.find(fullpath);
	if (it != Engine::asset_manager->cache_material.end())
	{
		return it->second;
	}
	else
	{
		shared_ptr<Material> mat;
		ifstream in_bin(fullpath, ios::binary);
		if (in_bin.is_open())
		{
			stringstream bin_s_stream;
			bin_s_stream << in_bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(mat);

			mat->Set_Texture_All();
			mat->Set_Shader_All();
			Engine::asset_manager->Registration_Asset(mat);
			Engine::asset_manager->cache_material.insert(make_pair(fullpath, mat));
			return mat;
		}
	}

	return nullptr;
}

void Material::Set_Texture(Texture_Type texture_type, const std::string& filepath, const string& filename)
{
	texture_info[static_cast<int>(texture_type)].texture_name = filename;
	texture_info[static_cast<int>(texture_type)].texture_pass = filepath;
	texture_info[static_cast<int>(texture_type)].texture_fullpass = filepath + filename;
	texture[static_cast<int>(texture_type)] = Texture::Load(texture_info[static_cast<int>(texture_type)].texture_fullpass);
}

void Material::Set_Shader(const string& shader_path, Shader::Shader_Type shader_type)
{
	int type = static_cast<int>(shader_type);
	if (shader_path.empty())
	{
		shader[type].reset();
		shader_info[type].shader_fullpass.clear();
		shader_info[type].shader_pass.clear(); //ファイルまでのディレクトリ
		shader_info[type].shader_name.clear();
	}
	else
	{
		int path_i = shader_path.find_last_of("\\") + 1;//7
		int ext_i = shader_path.find_last_of(".");//10
		shader_info[type].shader_fullpass = shader_path;
		shader_info[type].shader_pass = shader_path.substr(0, path_i); //ファイルまでのディレクトリ
		string filename = shader_path.substr(path_i, ext_i - path_i); //ファイル名
		shader_info[type].shader_name = filename;
		shader[type] = Shader::Create(shader_path, shader_type);
	}
}

void Material::Set_Shader_All()
{
	for (size_t i = 0; i < 5; i++)
	{
		if (!shader_info[i].shader_fullpass.empty())
		{
			shader[i] = Shader::Create(shader_info[i].shader_fullpass, static_cast<Shader::Shader_Type>(i));
		}
	}
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
	DxSystem::device_context->VSSetShader(nullptr, nullptr, 0);
	DxSystem::device_context->GSSetShader(nullptr, nullptr, 0);
	DxSystem::device_context->PSSetShader(nullptr, nullptr, 0);
	DxSystem::device_context->HSSetShader(nullptr, nullptr, 0);
	DxSystem::device_context->DSSetShader(nullptr, nullptr, 0);

	for (auto s : shader)
	{
		if (s)
		{
			s->Activate();
		}
	}
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
		self_save_pass = path;
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
		if (ImGui::TreeNode(u8"テクスチャ"))
		{
			static const char* tex_type[] = { "Main::","Specular::","Normal::","Height::","Emission::" };
			for (int i = 0; i < 5; ++i)
			{
				ImGui::Text(tex_type[i]);
				ImGui::SameLine();
				ImGui::Text(texture_info[i].texture_name.c_str());
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 30.0f);
				if (ImGui::Button(u8"選択"))
				{
					string path = System_Function::Get_Open_File_Name("png", "\\Resouces\\Model");
					if (path != "")
					{
						int path_i = path.find_last_of("\\") + 1;//7
						int ext_i = path.find_last_of(".");//10
						string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
						string extname = path.substr(ext_i, path.size() - ext_i); //拡張子
						string filename = path.substr(path_i, ext_i - path_i); //ファイル名
						Set_Texture(static_cast<Texture_Type>(i), pathname, filename + extname);
						Save();
					}
				}
			}
			ImGui::TreePop();
		}
		float out_color[4] = { color.x,color.y,color.z,color.w };
		if (ImGui::ColorEdit4("Color", out_color))
		{
			color = { out_color[0],out_color[1] ,out_color[2] ,out_color[3] };
			Save();
		}

		ImGui::Dummy(ImVec2(0, 3));

		static const char* depth[] = { "None", "Less", "Greater", "LEqual", "GEqual", "Equal", "NotEqual", "Always", "None_No_Write", "Less_No_Write", "Greater_No_Write", "LEqual_No_Write", "GEqual_No_Write", "Equal_No_Write", "NotEqual_No_Write", "Always_No_Write" };
		int depth_current = static_cast<int>(depth_stencil_state);
		if (ImGui::Combo("DepthMode", &depth_current, depth, IM_ARRAYSIZE(depth)))
		{
			depth_stencil_state = static_cast<DS_State>(depth_current);
			Save();
		}

		ImGui::Dummy(ImVec2(0, 3));

		static const char* cull[] = { "Back", "Front", "None" };
		int cull_current = static_cast<int>(rasterizer_state);
		if (ImGui::Combo("Culling", &cull_current, cull, IM_ARRAYSIZE(cull)))
		{
			rasterizer_state = static_cast<RS_State>(cull_current);
			Save();
		}

		ImGui::Dummy(ImVec2(0, 3));

		static const char* blends[] = { "Off", "Alpha", "Alpha_Test", "Transparent", "Add", "Subtract", "Replace", "Multiply" };
		int blend_current = static_cast<int>(blend_state);
		if (ImGui::Combo("BlendMode", &blend_current, blends, IM_ARRAYSIZE(blends)))
		{
			blend_state = static_cast<BS_State>(blend_current);
			Save();
		}
		ImGui::TreePop();
	}
}