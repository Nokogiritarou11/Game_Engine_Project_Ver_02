#include <clocale>
#include <unordered_map>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include "Debug.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "System_Function.h"
using namespace std;
using namespace BeastEngine;

shared_ptr<Material> Material::Create(const string& vertex_path, const string& pixel_path, const string& geometry_path)
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

			mat->Initialize_Texture();
			mat->Initialize_Shader();
			Engine::asset_manager->Registration_Asset(mat);
			Engine::asset_manager->cache_material.insert(make_pair(fullpath, mat));
			return mat;
		}
	}

	return nullptr;
}

void Material::Set_Texture(const string& texture_name, const shared_ptr<Texture>& texture)
{
	auto it = texture_info.find(texture_name);
	if (it == texture_info.end())
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
	else
	{
		it->second.texture = texture;
		it->second.texture_path = texture->Get_Path();
	}
}

void Material::Set_Parameter(const std::string& parameter_name, void* value, const Shader::Parameter_Type& type)
{
	auto it = parameter_info.find(parameter_name);
	if (it == parameter_info.end())
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
	else
	{
		Parameter_Info& info = it->second;
		if (info.type == type)
		{
			// 定数バッファ更新
			ConstantBuffer_Info& b_info = constant_buffer_info.find(info.parent_name)->second;
			memcpy(&b_info.byte_data[info.offset], value, info.size);
			{
				UINT subresourceIndex = 0;
				D3D11_MAPPED_SUBRESOURCE mapped;
				auto hr = DxSystem::device_context->Map(b_info.constant_buffer.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
				if (SUCCEEDED(hr))
				{
					memcpy(mapped.pData, &b_info.byte_data[0], b_info.byte_data.size());
					DxSystem::device_context->Unmap(b_info.constant_buffer.Get(), subresourceIndex);
				}
			}
		}
		else
		{
			Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
		}

	}
}

void Material::Set_Int(const string& int_name, int& value)
{
	Set_Parameter(int_name, &value, Shader::Parameter_Type::INT);
}

void Material::Set_Float(const string& float_name, float& value)
{
	Set_Parameter(float_name, &value, Shader::Parameter_Type::FLOAT);
}

void Material::Set_Vector2(const string& vector_name, Vector2& value)
{
	Set_Parameter(vector_name, &value, Shader::Parameter_Type::VECTOR2);
}

void Material::Set_Vector3(const string& vector_name, Vector3& value)
{
	Set_Parameter(vector_name, &value, Shader::Parameter_Type::VECTOR3);
}

void Material::Set_Vector4(const string& vector_name, Vector4& value)
{
	Set_Parameter(vector_name, &value, Shader::Parameter_Type::VECTOR4);
}

void Material::Set_Matrix(const string& matrix_name, Matrix& value)
{
	Set_Parameter(matrix_name, &value, Shader::Parameter_Type::MATRIX);
}

void Material::Reflect_Shader()
{
	constant_buffer_info.clear();
	parameter_info.clear();
	texture_info.clear();

	for (size_t i = 0; i < 5; ++i)
	{
		if (shared_ptr<Shader>& shader = shader_info[i].shader)
		{
			// コンスタントバッファ
			for (auto& c_info : shader->constant_buffer_info)
			{
				auto it = constant_buffer_info.find(c_info.name);
				if (it == constant_buffer_info.end())
				{
					ConstantBuffer_Info c;
					auto& cb_info = constant_buffer_info[c_info.name] = c;

					cb_info.register_number = c_info.register_number;
					cb_info.byte_data.resize(c_info.byte_size);
					cb_info.staging_shader.emplace_back(static_cast<Shader::Shader_Type>(i));

					D3D11_BUFFER_DESC bd = {};
					bd.Usage = D3D11_USAGE_DYNAMIC;
					bd.ByteWidth = c_info.byte_size;
					bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
					bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
					bd.MiscFlags = 0;
					bd.StructureByteStride = 0;
					HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, cb_info.constant_buffer.GetAddressOf());
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

					//パラメータ
					for (auto& p_info : c_info.parameters)
					{
						auto itr = parameter_info.find(p_info.name);
						if (itr == parameter_info.end())
						{
							Parameter_Info p = { p_info.type, c_info.name, p_info.size, p_info.offset };
							parameter_info[p_info.name] = p;
						}
					}
				}
				else
				{
					it->second.staging_shader.emplace_back(static_cast<Shader::Shader_Type>(i));
				}
			}

			// テクスチャ
			for (auto& t_info : shader->texture_info)
			{
				auto it = texture_info.find(t_info.name);
				if (it == texture_info.end())
				{
					Texture_Info tex;
					tex.register_number = t_info.register_number;
					tex.staging_shader.emplace_back(static_cast<Shader::Shader_Type>(i));
					texture_info[t_info.name] = tex;
				}
				else
				{
					it->second.staging_shader.emplace_back(static_cast<Shader::Shader_Type>(i));
				}
			}
		}
	}
}

void Material::Set_Shader(const string& path, Shader::Shader_Type shader_type)
{
	int type = static_cast<int>(shader_type);
	Shader_Info& info = shader_info[type];

	bool update = false;

	if (path.empty())
	{
		if (info.shader) update = true;
		info.shader.reset();
		info.shader_path.clear();
	}
	else
	{
		if (info.shader_path != path)
		{
			info.shader_path = path;
			info.shader = Shader::Create(path, shader_type);
			update = true;
		}
	}

	if (update) Reflect_Shader();
}

void Material::Initialize_Shader()
{
	for (size_t i = 0; i < 5; ++i)
	{
		Shader_Info& info = shader_info[i];
		if (info.shader_path.empty())
		{
			info.shader.reset();
		}
		else
		{
			info.shader = Shader::Create(info.shader_path, static_cast<Shader::Shader_Type>(i));
		}
	}
}

void Material::Initialize_Texture()
{
	for (auto& texture : texture_info)
	{
		texture.second.texture = Texture::Load(texture.second.texture_path);
	}
}

void Material::Active()
{
	// コンスタントバッファセット
	for (auto& i : constant_buffer_info)
	{
		auto& info = i.second;
		for (auto& stage : info.staging_shader)
		{
			switch (stage)
			{
				case BeastEngine::Shader::Shader_Type::Vertex:
					DxSystem::device_context->VSSetConstantBuffers(info.register_number, 1, info.constant_buffer.GetAddressOf());
					break;
				case BeastEngine::Shader::Shader_Type::Geometry:
					break;
				case BeastEngine::Shader::Shader_Type::Pixel:
					break;
				case BeastEngine::Shader::Shader_Type::Hull:
					break;
				case BeastEngine::Shader::Shader_Type::Domain:
					break;
			}
		}
	}

	// テクスチャセット
	for (auto& texture : texture_info)
	{
		auto& t = texture.second;
		for (auto& stage : t.staging_shader)
		{
			t.texture->Set(t.register_number, stage);
		}
	}

	// シェーダーセット
	DxSystem::device_context->VSSetShader(nullptr, nullptr, 0);
	DxSystem::device_context->GSSetShader(nullptr, nullptr, 0);
	DxSystem::device_context->PSSetShader(nullptr, nullptr, 0);
	DxSystem::device_context->HSSetShader(nullptr, nullptr, 0);
	DxSystem::device_context->DSSetShader(nullptr, nullptr, 0);
	for (auto& info : shader_info)
	{
		if (info.shader)
		{
			info.shader->Activate();
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
	{/*
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
		}*/
		ImGui::TreePop();
	}
}