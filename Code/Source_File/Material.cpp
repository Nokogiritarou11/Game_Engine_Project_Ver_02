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

void Material::Set_Parameter(const string& parameter_name, void* value, const Shader::Parameter_Type& type)
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

shared_ptr<Texture> Material::Get_Texture(const string& texture_name)
{
	auto it = texture_info.find(texture_name);
	if (it == texture_info.end())
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
	else
	{
		return it->second.texture;
	}

	return nullptr;
}

int Material::Get_Int(const string& int_name)
{
	auto it = parameter_info.find(int_name);
	if (it == parameter_info.end())
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
	else
	{
		Parameter_Info& info = it->second;
		if (info.type == Shader::Parameter_Type::INT)
		{
			ConstantBuffer_Info& b_info = constant_buffer_info.find(info.parent_name)->second;
			int value = 0;
			memcpy(&value, &b_info.byte_data[info.offset], info.size);
			return value;
		}
		else
		{
			Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
		}
	}

	return 0;
}

float Material::Get_Float(const string& float_name)
{
	auto it = parameter_info.find(float_name);
	if (it == parameter_info.end())
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
	else
	{
		Parameter_Info& info = it->second;
		if (info.type == Shader::Parameter_Type::FLOAT)
		{
			ConstantBuffer_Info& b_info = constant_buffer_info.find(info.parent_name)->second;
			float value = 0;
			memcpy(&value, &b_info.byte_data[info.offset], info.size);
			return value;
		}
		else
		{
			Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
		}
	}

	return 0;
}

Vector2 Material::Get_Vector2(const string& vector_name)
{
	Vector2 value = { 0,0 };
	auto it = parameter_info.find(vector_name);
	if (it == parameter_info.end())
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
	else
	{
		Parameter_Info& info = it->second;
		if (info.type == Shader::Parameter_Type::VECTOR2)
		{
			ConstantBuffer_Info& b_info = constant_buffer_info.find(info.parent_name)->second;
			memcpy(&value, &b_info.byte_data[info.offset], info.size);
			return value;
		}
		else
		{
			Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
		}
	}

	return value;
}

Vector3 Material::Get_Vector3(const string& vector_name)
{
	Vector3 value = { 0,0,0 };
	auto it = parameter_info.find(vector_name);
	if (it == parameter_info.end())
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
	else
	{
		Parameter_Info& info = it->second;
		if (info.type == Shader::Parameter_Type::VECTOR3)
		{
			ConstantBuffer_Info& b_info = constant_buffer_info.find(info.parent_name)->second;
			memcpy(&value, &b_info.byte_data[info.offset], info.size);
			return value;
		}
		else
		{
			Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
		}
	}

	return value;
}

Vector4 Material::Get_Vector4(const string& vector_name)
{
	Vector4 value = { 0,0,0,0 };
	auto it = parameter_info.find(vector_name);
	if (it == parameter_info.end())
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
	else
	{
		Parameter_Info& info = it->second;
		if (info.type == Shader::Parameter_Type::VECTOR4)
		{
			ConstantBuffer_Info& b_info = constant_buffer_info.find(info.parent_name)->second;
			memcpy(&value, &b_info.byte_data[info.offset], info.size);
			return value;
		}
		else
		{
			Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
		}
	}

	return value;
}

Matrix Material::Get_Matrix(const string& matrix_name)
{
	Matrix value = Matrix::Identity;
	auto it = parameter_info.find(matrix_name);
	if (it == parameter_info.end())
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
	else
	{
		Parameter_Info& info = it->second;
		if (info.type == Shader::Parameter_Type::MATRIX)
		{
			ConstantBuffer_Info& b_info = constant_buffer_info.find(info.parent_name)->second;
			memcpy(&value, &b_info.byte_data[info.offset], info.size);
			return value;
		}
		else
		{
			Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
		}
	}

	return value;
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

					Create_ConstantBuffer(cb_info, c_info.byte_size);

					//パラメータ
					for (auto& p_info : c_info.parameters)
					{
						auto itr = parameter_info.find(p_info.name);
						if (itr == parameter_info.end())
						{
							void* data = &cb_info.byte_data[p_info.offset];
							if (p_info.type == Shader::Parameter_Type::INT)
							{
								int value = 1;
								memcpy(data, &value, p_info.size);
							}
							else if (p_info.type == Shader::Parameter_Type::FLOAT)
							{
								float value = 1.0f;
								memcpy(data, &value, p_info.size);
							}
							else if (p_info.type == Shader::Parameter_Type::VECTOR2)
							{
								Vector2 value = { 1.0f, 1.0f };
								memcpy(data, &value, p_info.size);
							}
							else if (p_info.type == Shader::Parameter_Type::VECTOR3)
							{
								Vector3 value = { 1.0f, 1.0f, 1.0f };
								memcpy(data, &value, p_info.size);
							}
							else if (p_info.type == Shader::Parameter_Type::VECTOR4)
							{
								Vector4 value = { 1.0f, 1.0f, 1.0f, 1.0f };
								memcpy(data, &value, p_info.size);
							}
							else if (p_info.type == Shader::Parameter_Type::MATRIX)
							{
								Matrix value = Matrix::Identity;
								memcpy(data, &value, p_info.size);
							}
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

void Material::Create_ConstantBuffer(ConstantBuffer_Info& info, const UINT& size)
{
	HRESULT hr;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = DxSystem::device->CreateBuffer(&bd, nullptr, info.constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	UINT subresourceIndex = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = DxSystem::device_context->Map(info.constant_buffer.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &info.byte_data[0], info.byte_data.size());
		DxSystem::device_context->Unmap(info.constant_buffer.Get(), subresourceIndex);
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

		if (info.shader)
		{
			if (shared_ptr<Shader>& shader = info.shader)
			{
				// コンスタントバッファ作成
				for (auto& cb_info : constant_buffer_info)
				{
					Create_ConstantBuffer(cb_info.second, sizeof(std::byte) * cb_info.second.byte_data.size());
				}
			}
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
				case Shader::Shader_Type::Vertex:
					DxSystem::device_context->VSSetConstantBuffers(info.register_number, 1, info.constant_buffer.GetAddressOf());
					break;
				case Shader::Shader_Type::Geometry:
					DxSystem::device_context->GSSetConstantBuffers(info.register_number, 1, info.constant_buffer.GetAddressOf());
					break;
				case Shader::Shader_Type::Pixel:
					DxSystem::device_context->PSSetConstantBuffers(info.register_number, 1, info.constant_buffer.GetAddressOf());
					break;
				case Shader::Shader_Type::Hull:
					DxSystem::device_context->HSSetConstantBuffers(info.register_number, 1, info.constant_buffer.GetAddressOf());
					break;
				case Shader::Shader_Type::Domain:
					DxSystem::device_context->DSSetConstantBuffers(info.register_number, 1, info.constant_buffer.GetAddressOf());
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
	{
		const float window_width = ImGui::GetWindowContentRegionWidth();

		if (ImGui::TreeNode(u8"シェーダー"))
		{
			static const char* s_name[] = { "VS : ", "GS : ", "PS : ", "HS : ", "DS : " };
			for (size_t i = 0; i < 5; ++i)
			{
				ImGui::PushID(i);
				const string& path = shader_info[i].shader_path;
				const int& path_i = path.find_last_of("\\") + 1;
				const string& filename = path.substr(path_i, path.size()); //ファイル名

				ImGui::Text(s_name[i]);
				ImGui::SameLine();
				ImGui::Text(filename.c_str());
				ImGui::SameLine(window_width - 25.0f);
				if (ImGui::Button(u8"選択"))
				{
					const string& path = System_Function::Get_Open_File_Name("png", "\\Resouces\\Image");
					if (path != "")
					{
						Set_Shader(path, static_cast<Shader::Shader_Type>(i));
						Save();
					}
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		//テクスチャ
		const ImVec2& size = ImVec2(100.0f, 100.0f);
		const ImVec2& uv0 = ImVec2(0.0f, 0.0f);
		const ImVec2& uv1 = ImVec2(1.0f, 1.0f);
		const int& frame_padding = 3;
		const ImVec4& bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		const ImVec4& tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		for (auto& info : texture_info)
		{
			ImGui::Text(info.first.c_str());
			const auto& tex = info.second.texture;
			ImGui::SameLine(window_width - size.x - frame_padding);
			if (ImGui::ImageButton((void*)tex->Get_Resource().Get(), size, uv0, uv1, frame_padding, bg_col, tint_col))
			{
				string path = System_Function::Get_Open_File_Name("png", "\\Resouces\\Model");
				if (!path.empty())
				{
					Set_Texture(info.first, Texture::Load(path));
					Save();
				}
			}
		}

		// パラメータ
		for (auto& info : parameter_info)
		{
			const string& p_name = info.first;
			ImGui::PushID(p_name.c_str());
			ImGui::Text(p_name.c_str());

			ImGui::SameLine(window_width * 0.4f);
			ImGui::SetNextItemWidth(-FLT_MIN);

			if (info.second.type == Shader::Parameter_Type::INT)
			{
				int value = Get_Int(p_name);
				if (ImGui::DragInt("##int", &value))
				{
					Set_Int(p_name, value);
					Save();
				}
			}
			else if (info.second.type == Shader::Parameter_Type::FLOAT)
			{
				float value = Get_Float(p_name);
				if (ImGui::DragFloat("##float", &value))
				{
					Set_Float(p_name, value);
					Save();
				}
			}
			else if (info.second.type == Shader::Parameter_Type::VECTOR2)
			{
				Vector2 value = Get_Vector2(p_name);
				float v[2] = { value.x, value.y };
				if (ImGui::DragFloat2("##float2", v))
				{
					Vector2 r = { v[0], v[1] };
					Set_Vector2(p_name, r);
					Save();
				}
			}
			else if (info.second.type == Shader::Parameter_Type::VECTOR3)
			{
				Vector3 value = Get_Vector3(p_name);
				float v[3] = { value.x, value.y, value.z };
				if (ImGui::DragFloat3("##float3", v))
				{
					Vector3 r = { v[0], v[1],v[2] };
					Set_Vector3(p_name, r);
					Save();
				}
			}
			else if (info.second.type == Shader::Parameter_Type::VECTOR4)
			{
				Vector4 value = Get_Vector4(p_name);
				float v[4] = { value.x, value.y, value.z, value.w };
				if (ImGui::ColorEdit4("##float4_color", v))
				{
					Vector4 r = { v[0], v[1], v[2], v[3] };
					Set_Vector4(p_name, r);
					Save();
				}
				ImGui::Dummy({ 0,0 });
				ImGui::SameLine(window_width * 0.4f);
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (ImGui::DragFloat4("##float4_color", v))
				{
					Vector4 r = { v[0], v[1], v[2], v[3] };
					Set_Vector4(p_name, r);
					Save();
				}
			}
			else if (info.second.type == Shader::Parameter_Type::MATRIX)
			{
				ImGui::Dummy({ 0,0 }); // GUI非対応
			}
			ImGui::PopID();
		}

		static const char* depth[] = { "None", "Less", "Greater", "LEqual", "GEqual", "Equal", "NotEqual", "Always", "None_No_Write", "Less_No_Write", "Greater_No_Write", "LEqual_No_Write", "GEqual_No_Write", "Equal_No_Write", "NotEqual_No_Write", "Always_No_Write" };
		int depth_current = static_cast<int>(depth_stencil_state);
		ImGui::Text(u8"深度設定");
		ImGui::SameLine(window_width * 0.6f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::Combo("##DepthMode", &depth_current, depth, IM_ARRAYSIZE(depth)))
		{
			depth_stencil_state = static_cast<DS_State>(depth_current);
			Save();
		}

		ImGui::Text(u8"カリング");
		static const char* cull[] = { "Back", "Front", "None" };
		int cull_current = static_cast<int>(rasterizer_state);
		ImGui::SameLine(window_width * 0.6f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::Combo("##Culling", &cull_current, cull, IM_ARRAYSIZE(cull)))
		{
			rasterizer_state = static_cast<RS_State>(cull_current);
			Save();
		}

		ImGui::Text(u8"ブレンド");
		static const char* blends[] = { "Off", "Alpha", "Alpha_Test", "Transparent", "Add", "Subtract", "Replace", "Multiply" };
		int blend_current = static_cast<int>(blend_state);
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.6f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::Combo("##BlendMode", &blend_current, blends, IM_ARRAYSIZE(blends)))
		{
			blend_state = static_cast<BS_State>(blend_current);
			Save();
		}

		ImGui::TreePop();
	}
}