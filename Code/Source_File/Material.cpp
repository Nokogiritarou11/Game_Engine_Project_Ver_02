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
#include "Misc.h"
#include "System_Function.h"
using namespace std;
using namespace BeastEngine;

BS_State Material::binding_blend_state = BS_State::Off;
RS_State Material::binding_rasterizer_state = RS_State::Cull_None;
DS_State Material::binding_depth_stencil_state = DS_State::LEqual;

shared_ptr<Material> Material::Create(const string& vertex_path, const string& pixel_path, const string& geometry_path)
{
	shared_ptr<Material> mat = make_shared<Material>();
	mat->name = "material";
	mat->Add_Pass(vertex_path, pixel_path, geometry_path);
	Engine::asset_manager->Registration_Asset(mat);
	return mat;
}

shared_ptr<Material> Material::Load_Material(const string& full_path)
{
	if (const auto it = Engine::asset_manager->cache_material.find(full_path); it != Engine::asset_manager->cache_material.end())
	{
		return it->second;
	}

	if (const ifstream in_bin(full_path, ios::binary); in_bin.is_open())
	{
		shared_ptr<Material> mat;
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(mat);

		for (auto& pass : mat->render_pass)
		{
			pass.Initialize_Shader();
			pass.Initialize_Texture();
		}

		const int path_i = full_path.find_last_of("\\") + 1;//7
		const int ext_i = full_path.find_last_of(".");//10
		const string filename = full_path.substr(path_i, ext_i - path_i); //ファイル名
		mat->name = filename;

		if (!mat->self_save_path.empty())
		{
			mat->self_save_path = full_path;
			mat->Save();
		}
		Engine::asset_manager->Registration_Asset(mat);
		Engine::asset_manager->cache_material.insert(make_pair(full_path, mat));
		return mat;
	}

	return nullptr;
}

void Material::Add_Pass(const std::string& vertex_path, const std::string& pixel_path, const std::string& geometry_path)
{
	render_pass.emplace_back();
	const int pass = static_cast<int>(render_pass.size()) - 1;
	Set_Shader(vertex_path, Shader::Shader_Type::Vertex, pass);
	Set_Shader(pixel_path, Shader::Shader_Type::Pixel, pass);
	Set_Shader(geometry_path, Shader::Shader_Type::Geometry, pass);
}

void Material::Remove_Pass()
{
	if (render_pass.size() > 1) render_pass.pop_back();
}

void Material::Set_Rendering_Mode(Rendering_Mode mode)
{
	rendering_mode = mode;
	if (rendering_mode == Rendering_Mode::Opaque)
	{
		render_queue = 2000;
		for (auto& pass : render_pass)
		{
			pass.blend_state = BS_State::Off;
			pass.depth_stencil_state = DS_State::GEqual;
		}
	}
	else if (rendering_mode == Rendering_Mode::Cutout)
	{
		render_queue = 2450;
		for (auto& pass : render_pass)
		{
			pass.blend_state = BS_State::Alpha_Test;
			pass.depth_stencil_state = DS_State::GEqual;
		}
	}
	else
	{
		render_queue = 3000;
		for (auto& pass : render_pass)
		{
			pass.blend_state = BS_State::Alpha;
			pass.depth_stencil_state = DS_State::GEqual_No_Write;
		}
	}
}

void Material::Set_Blend_State(BS_State state, int pass)
{
	render_pass[pass].blend_state = state;
}

void Material::Set_Rasterizer_State(RS_State state, int pass)
{
	render_pass[pass].rasterizer_state = state;
}

void Material::Set_Depth_Stencil_State(DS_State state, int pass)
{
	render_pass[pass].depth_stencil_state = state;
}

void Material::Set_Shader(const string& path, Shader::Shader_Type shader_type, int pass)
{
	Render_Pass& r_pass = render_pass[pass];
	Shader_Info& info = r_pass.shader_info[static_cast<int>(shader_type)];

	bool update = false;
	bool newer = false;

	if (path.empty())
	{
		if (info.shader)
		{
			update = true;
			info.shader.reset();
			info.shader_path.clear();
		}
	}
	else
	{
		if (info.shader_path != path)
		{
			if (!info.shader) newer = true;
			info.shader_path = path;
			info.shader = Shader::Create(path, shader_type);
			update = true;
		}
	}

	if (update)
	{
		if (newer)
		{
			r_pass.Reflect_Shader(shader_type);
			r_pass.Reflect_Texture(shader_type);
		}
		else
		{
			r_pass.Reflect_Shader();
			r_pass.Reflect_Texture();
		}
		r_pass.Initialize_Shader();
		r_pass.Initialize_Texture();
	}
}

void Material::Set_Texture(const string& texture_name, const shared_ptr<Texture>& texture)
{
	bool change = false;
	for (auto& pass : render_pass)
	{
		auto it = pass.texture_info.find(texture_name);
		if (it != pass.texture_info.end())
		{
			auto& info = it->second;
			info.texture = texture;
			info.texture_path = texture->Get_Path();

			if (info.register_number == 1)
			{
				for (const auto& stage : info.staging_shader)
				{
					if (stage == Shader::Shader_Type::Pixel)
					{
						pass.main_texture = info.texture;
					}
				}
			}
			change = true;
		}
	}

	if (!change)
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
}

void Material::Set_Parameter(const string& parameter_name, const void* value, const Shader::Parameter_Type& type)
{
	bool change = false;
	for (auto& pass : render_pass)
	{
		if (const auto it = pass.parameter_info.find(parameter_name); it != pass.parameter_info.end())
		{
			Parameter_Info& info = it->second;
			if (info.type == type)
			{
				// 定数バッファ更新
				ConstantBuffer_Info& b_info = pass.constant_buffer_info.find(info.parent_name)->second;
				memcpy(&b_info.byte_data[info.offset], value, info.size);
				{
					constexpr UINT subresource_index = 0;
					D3D11_MAPPED_SUBRESOURCE mapped;
					auto hr = DxSystem::device_context->Map(b_info.constant_buffer.Get(), subresource_index, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
					if (SUCCEEDED(hr))
					{
						memcpy(mapped.pData, &b_info.byte_data[0], b_info.byte_data.size());
						DxSystem::device_context->Unmap(b_info.constant_buffer.Get(), subresource_index);
					}
				}
				change = true;
			}
		}
	}

	if (!change)
	{
		Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	}
}

void Material::Set_Int(const string& int_name, const int& value)
{
	Set_Parameter(int_name, &value, Shader::Parameter_Type::Int);
}

void Material::Set_Float(const string& float_name, const float& value)
{
	Set_Parameter(float_name, &value, Shader::Parameter_Type::Float);
}

void Material::Set_Vector2(const string& vector_name, const Vector2& value)
{
	Set_Parameter(vector_name, &value, Shader::Parameter_Type::Vector2);
}

void Material::Set_Vector3(const string& vector_name, const Vector3& value)
{
	Set_Parameter(vector_name, &value, Shader::Parameter_Type::Vector3);
}

void Material::Set_Vector4(const string& vector_name, const Vector4& value)
{
	Set_Parameter(vector_name, &value, Shader::Parameter_Type::Vector4);
}

void Material::Set_Matrix(const string& matrix_name, const Matrix& value)
{
	Set_Parameter(matrix_name, &value, Shader::Parameter_Type::Matrix);
}

shared_ptr<Texture> Material::Get_Texture(const string& texture_name)
{
	for (auto& pass : render_pass)
	{
		if (const auto it = pass.texture_info.find(texture_name); it != pass.texture_info.end())
		{
			return it->second.texture;
		}
	}

	Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	return nullptr;
}

int Material::Get_Int(const string& int_name)
{
	for (auto& pass : render_pass)
	{
		if (const auto it = pass.parameter_info.find(int_name); it != pass.parameter_info.end())
		{
			if (const Parameter_Info& info = it->second; info.type == Shader::Parameter_Type::Int)
			{
				ConstantBuffer_Info& b_info = pass.constant_buffer_info.find(info.parent_name)->second;
				int value = 0;
				memcpy(&value, &b_info.byte_data[info.offset], info.size);
				return value;
			}
		}
	}

	Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	return 0;
}

float Material::Get_Float(const string& float_name)
{
	for (auto& pass : render_pass)
	{
		if (const auto it = pass.parameter_info.find(float_name); it != pass.parameter_info.end())
		{
			if (const Parameter_Info& info = it->second; info.type == Shader::Parameter_Type::Float)
			{
				const ConstantBuffer_Info& b_info = pass.constant_buffer_info.find(info.parent_name)->second;
				float value = 0;
				memcpy(&value, &b_info.byte_data[info.offset], info.size);
				return value;
			}
		}
	}

	Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	return 0;
}

Vector2 Material::Get_Vector2(const string& vector_name)
{
	Vector2 value = { 0,0 };

	for (auto& pass : render_pass)
	{
		if (const auto it = pass.parameter_info.find(vector_name); it != pass.parameter_info.end())
		{
			if (const Parameter_Info& info = it->second; info.type == Shader::Parameter_Type::Vector2)
			{
				ConstantBuffer_Info& b_info = pass.constant_buffer_info.find(info.parent_name)->second;
				memcpy(&value, &b_info.byte_data[info.offset], info.size);
				return value;
			}
		}
	}

	Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	return value;
}

Vector3 Material::Get_Vector3(const string& vector_name)
{
	Vector3 value = { 0,0,0 };
	for (auto& pass : render_pass)
	{
		if (const auto it = pass.parameter_info.find(vector_name); it != pass.parameter_info.end())
		{
			if (const Parameter_Info& info = it->second; info.type == Shader::Parameter_Type::Vector3)
			{
				const ConstantBuffer_Info& b_info = pass.constant_buffer_info.find(info.parent_name)->second;
				memcpy(&value, &b_info.byte_data[info.offset], info.size);
				return value;
			}
		}
	}

	Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	return value;
}

Vector4 Material::Get_Vector4(const string& vector_name)
{
	Vector4 value = { 0,0,0,0 };
	for (auto& pass : render_pass)
	{
		if (const auto it = pass.parameter_info.find(vector_name); it != pass.parameter_info.end())
		{
			if (const Parameter_Info& info = it->second; info.type == Shader::Parameter_Type::Vector4)
			{
				const ConstantBuffer_Info& b_info = pass.constant_buffer_info.find(info.parent_name)->second;
				memcpy(&value, &b_info.byte_data[info.offset], info.size);
				return value;
			}
		}
	}

	Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	return value;
}

Matrix Material::Get_Matrix(const string& matrix_name)
{
	Matrix value = Matrix::Identity;
	for (auto& pass : render_pass)
	{
		if (const auto it = pass.parameter_info.find(matrix_name); it != pass.parameter_info.end())
		{
			if (const Parameter_Info& info = it->second; info.type == Shader::Parameter_Type::Matrix)
			{
				const ConstantBuffer_Info& b_info = pass.constant_buffer_info.find(info.parent_name)->second;
				memcpy(&value, &b_info.byte_data[info.offset], info.size);
				return value;
			}
		}
	}

	Debug::Log(u8"指定したシェーダーパラメーターが見つかりません");
	return value;
}

void Material::Render_Pass::Reflect_Shader()
{
	std::unordered_map<std::string, ConstantBuffer_Info> copy_constant_buffer_info = constant_buffer_info;
	std::unordered_map<std::string, Parameter_Info> copy_parameter_info = parameter_info;

	constant_buffer_info.clear();
	parameter_info.clear();

	for (size_t i = 0; i < 5; ++i)
	{
		if (const auto& shader = shader_info[i].shader)
		{
			// コンスタントバッファ
			for (auto& c_info : shader->constant_buffer_info)
			{
				if (const auto it = constant_buffer_info.find(c_info.name); it == constant_buffer_info.end())
				{
					const ConstantBuffer_Info c;
					auto& cb_info = constant_buffer_info[c_info.name] = c;

					cb_info.register_number = c_info.register_number;
					cb_info.byte_data.resize(c_info.byte_size);
					cb_info.staging_shader.emplace_back(static_cast<Shader::Shader_Type>(i));

					//パラメータ
					for (auto& p_info : c_info.parameters)
					{
						auto itr = parameter_info.find(p_info.name);
						if (itr == parameter_info.end())
						{
							void* data = &cb_info.byte_data[p_info.offset];

							bool initialized = false;
							auto copy_it = copy_parameter_info.find(p_info.name);
							if (copy_it != copy_parameter_info.end())
							{
								if (copy_it->second.type == p_info.type)
								{
									//変更前のデーターをコピー
									ConstantBuffer_Info& b_info = copy_constant_buffer_info.find(copy_it->second.parent_name)->second;
									memcpy(data, &b_info.byte_data[copy_it->second.offset], p_info.size);
									initialized = true;
								}
							}

							//初期値代入
							if (!initialized)
							{
								memcpy(data, &p_info.default_value[0], p_info.size);
							}
							//作成
							const Parameter_Info p = { p_info.type, c_info.name, p_info.size, p_info.offset };
							parameter_info[p_info.name] = p;
						}
					}
				}
				else
				{
					it->second.staging_shader.emplace_back(static_cast<Shader::Shader_Type>(i));
				}
			}
		}
	}
}

void Material::Render_Pass::Reflect_Shader(Shader::Shader_Type type)
{
	if (const auto& shader = shader_info[static_cast<int>(type)].shader)
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
				cb_info.staging_shader.emplace_back(type);

				//パラメータ
				for (auto& p_info : c_info.parameters)
				{
					if (auto itr = parameter_info.find(p_info.name); itr == parameter_info.end())
					{
						//初期値代入
						void* data = &cb_info.byte_data[p_info.offset];
						memcpy(data, &p_info.default_value[0], p_info.size);
						const Parameter_Info p = { p_info.type, c_info.name, p_info.size, p_info.offset };
						parameter_info[p_info.name] = p;
					}
				}
			}
			else
			{
				it->second.staging_shader.emplace_back(type);
			}
		}
	}
}

void Material::Render_Pass::Reflect_Texture()
{
	auto copy_texture_info = texture_info;
	texture_info.clear();

	for (size_t i = 0; i < 5; ++i)
	{
		if (const auto& shader = shader_info[i].shader)
		{
			// テクスチャ
			for (auto& t_info : shader->texture_info)
			{
				auto it = texture_info.find(t_info.name);
				if (it == texture_info.end())
				{
					Texture_Info tex;
					tex.register_number = t_info.register_number;
					tex.staging_shader.emplace_back(static_cast<Shader::Shader_Type>(i));

					// 変更前に存在するか確認
					auto copy_it = copy_texture_info.find(t_info.name);
					if (copy_it == copy_texture_info.end())
					{
						tex.texture_path = "Default_Assets\\Image\\Default_Texture.png";
					}
					else
					{
						tex.texture_path = copy_it->second.texture_path;
					}
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

void Material::Render_Pass::Reflect_Texture(Shader::Shader_Type type)
{
	if (const auto& shader = shader_info[static_cast<int>(type)].shader)
	{
		// テクスチャ
		for (auto& t_info : shader->texture_info)
		{
			auto it = texture_info.find(t_info.name);
			if (it == texture_info.end())
			{
				Texture_Info tex;
				tex.register_number = t_info.register_number;
				tex.staging_shader.emplace_back(type);
				tex.texture_path = "Default_Assets\\Image\\Default_Texture.png";
				texture_info[t_info.name] = tex;
			}
			else
			{
				it->second.staging_shader.emplace_back(type);
			}
		}
	}
}

void Material::Render_Pass::Create_ConstantBuffer(ConstantBuffer_Info& info, const UINT& size)
{
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, info.constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	constexpr UINT subresource_index = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = DxSystem::device_context->Map(info.constant_buffer.Get(), subresource_index, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &info.byte_data[0], info.byte_data.size());
		DxSystem::device_context->Unmap(info.constant_buffer.Get(), subresource_index);
	}
}

void Material::Render_Pass::Initialize_Shader()
{
	bool change = false;
	for (size_t i = 0; i < 5; ++i)
	{
		if (shader_info[i].shader_path.empty()) continue;
		shader_info[i].shader = Shader::Create(shader_info[i].shader_path, static_cast<Shader::Shader_Type>(i));
		if (const auto& shader = shader_info[i].shader)
		{
			// コンスタントバッファ
			for (auto& c_info : shader->constant_buffer_info)
			{
				auto it = constant_buffer_info.find(c_info.name);
				if (it == constant_buffer_info.end())
				{
					change = true;
					break;
				}
				else
				{
					//パラメータ
					for (auto& p_info : c_info.parameters)
					{
						auto itr = parameter_info.find(p_info.name);
						if (itr == parameter_info.end())
						{
							change = true;
							break;
						}
						else
						{
							if (p_info.type != itr->second.type)
							{
								change = true;
								break;
							}
						}
					}
				}
				if (change) break;
			}
		}
		if (change) break;
	}

	if (change)
	{
		Reflect_Shader();
	}

	for (size_t i = 0; i < 5; ++i)
	{
		Shader_Info& info = shader_info[i];
		if (info.shader)
		{
			// コンスタントバッファ作成
			for (auto& cb_info : constant_buffer_info)
			{
				Create_ConstantBuffer(cb_info.second, sizeof(std::byte) * cb_info.second.byte_data.size());
			}
		}
	}
}

void Material::Render_Pass::Initialize_Texture()
{
	bool change = false;
	for (size_t i = 0; i < 5; ++i)
	{
		if (const auto& shader = shader_info[i].shader)
		{
			// テクスチャ
			for (auto& t_info : shader->texture_info)
			{
				auto it = texture_info.find(t_info.name);
				if (it == texture_info.end())
				{
					change = true;
					break;
				}
			}
		}
		if (change) break;
	}

	if (change)
	{
		Reflect_Texture();
	}

	for (auto& texture : texture_info)
	{
		auto& info = texture.second;
		info.texture = Texture::Load(info.texture_path);

		if (info.register_number == 1)
		{
			for (auto& stage : info.staging_shader)
			{
				if (stage == Shader::Shader_Type::Pixel)
				{
					main_texture = info.texture;
				}
			}
		}
	}
}

void Material::Render_Pass::Activate()
{
	Activate_Shader();
	Activate_Buffer();
	Activate_Texture();
	Activate_State();
}

void Material::Render_Pass::Activate_Buffer()
{
	// コンスタントバッファセット
	for (auto& i : constant_buffer_info)
	{
		auto& info = i.second;
		for (const auto& stage : info.staging_shader)
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
}

void Material::Render_Pass::Activate_Texture()
{
	// テクスチャセット
	for (auto& texture : texture_info)
	{
		auto& t = texture.second;
		for (const auto& stage : t.staging_shader)
		{
			t.texture->Set(t.register_number, stage);
		}
	}
}

void Material::Render_Pass::Activate_Shader()
{
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

void Material::Render_Pass::Activate_State() const
{
	//ブレンドステート設定
	if (binding_blend_state != blend_state)
	{
		DxSystem::device_context->OMSetBlendState(DxSystem::Get_Blend_State(blend_state), nullptr, 0xFFFFFFFF);
		binding_blend_state = blend_state;
	}
	//ラスタライザ―設定
	if (binding_rasterizer_state != rasterizer_state)
	{
		DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(rasterizer_state));
		binding_rasterizer_state = rasterizer_state;
	}
	//デプスステンシルステート設定
	if (binding_depth_stencil_state != depth_stencil_state)
	{
		DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DepthStencil_State(depth_stencil_state), 1);
		binding_depth_stencil_state = depth_stencil_state;
	}
}

void Material::Activate(int pass)
{
	render_pass[pass].Activate();
}

void Material::Activate_Buffer(int pass)
{
	render_pass[pass].Activate_Shader();
}

void Material::Activate_Texture(int pass)
{
	render_pass[pass].Activate_Texture();
}

void Material::Activate_Shader(int pass)
{
	render_pass[pass].Activate_Shader();
}

void Material::Activate_State(int pass)
{
	render_pass[pass].Activate_State();
}

void Material::Save(const string& path)
{
	string save_path;
	if (path.empty())
	{
		save_path = self_save_path;
	}
	else
	{
		save_path = path;
		self_save_path = path;
	}
	{
		ofstream ss(save_path, ios::binary);
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(static_pointer_cast<Material>(shared_from_this()));
	}
}

void Material::Draw_ImGui()
{
	const float window_width = ImGui::GetWindowContentRegionWidth();

	static const char* mode_name[] = { "Opaque", "Cutout", "Transparent" };
	int mode_current = static_cast<int>(rendering_mode);
	if (ImGui::LeftText_Combo(u8"描画タイプ", "##Rendering_Mode", &mode_current, mode_name, IM_ARRAYSIZE(mode_name), window_width * 0.6f))
	{
		Set_Rendering_Mode(static_cast<Rendering_Mode>(mode_current));
		Save();
	}

	for (size_t i = 0; i < render_pass.size(); ++i)
	{
		Render_Pass& pass = render_pass[i];
		const string pass_label = to_string(i + 1) + u8"パス目";
		ImGui::PushID(pass_label.c_str());
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode(pass_label.c_str()))
		{
			if (ImGui::TreeNode(u8"シェーダー"))
			{
				static const char* s_name[] = { "VS : ", "GS : ", "PS : ", "HS : ", "DS : " };
				for (size_t t = 0; t < 5; ++t)
				{
					ImGui::PushID(t);
					const string& path = pass.shader_info[t].shader_path;
					const int& path_i = path.find_last_of("\\") + 1;
					const string& filename = path.substr(path_i, path.size()); //ファイル名

					ImGui::Text(s_name[t]);
					ImGui::SameLine();
					ImGui::Text(filename.c_str());
					ImGui::SameLine(window_width - 25.0f);
					if (ImGui::Button(u8"選択"))
					{
						if (const string& file_path = System_Function::Get_Open_File_Name("", "\\Shader"); file_path != "")
						{
							Set_Shader(file_path, static_cast<Shader::Shader_Type>(t), i);
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
			for (auto& info : pass.texture_info)
			{
				ImGui::PushID(info.first.c_str());
				ImGui::Text(info.first.c_str());
				const auto& tex = info.second.texture;
				ImGui::SameLine(window_width - size.x - frame_padding);
				if (ImGui::ImageButton(static_cast<void*>(tex->Get_Resource().Get()), size, uv0, uv1, frame_padding, bg_col, tint_col))
				{
					string path = System_Function::Get_Open_File_Name("png", "\\Assets\\Model");
					if (!path.empty())
					{
						Set_Texture(info.first, Texture::Load(path));
						Save();
					}
				}
				ImGui::PopID();
			}

			// パラメータ
			for (auto& info : pass.parameter_info)
			{
				const string& p_name = info.first;
				ImGui::PushID(p_name.c_str());

				if (info.second.type == Shader::Parameter_Type::Int)
				{
					int value = Get_Int(p_name);
					if (ImGui::LeftText_DragInt(p_name.c_str(), "##int", &value, window_width * 0.4f))
					{
						Set_Int(p_name, value);
						Save();
					}
				}
				else if (info.second.type == Shader::Parameter_Type::Float)
				{
					float value = Get_Float(p_name);
					if (ImGui::LeftText_DragFloat(p_name.c_str(), "##float", &value, window_width * 0.4f, -FLT_MIN, 0.001f, "%.5f"))
					{
						Set_Float(p_name, value);
						Save();
					}
				}
				else if (info.second.type == Shader::Parameter_Type::Vector2)
				{
					Vector2 value = Get_Vector2(p_name);
					if (ImGui::LeftText_DragFloat2(p_name.c_str(), "##float2", value, window_width * 0.4f, -FLT_MIN, 0.001f, "%.4f"))
					{
						Set_Vector2(p_name, value);
						Save();
					}
				}
				else if (info.second.type == Shader::Parameter_Type::Vector3)
				{
					Vector3 value = Get_Vector3(p_name);
					if (ImGui::LeftText_DragFloat3(p_name.c_str(), "##float3", value, window_width * 0.4f))
					{
						Set_Vector3(p_name, value);
						Save();
					}
				}
				else if (info.second.type == Shader::Parameter_Type::Vector4)
				{
					Vector4 value = Get_Vector4(p_name);
					if (ImGui::LeftText_ColorEdit4(p_name.c_str(), "##float4_color", value, window_width * 0.4f))
					{
						Set_Vector4(p_name, value);
						Save();
					}
					if (ImGui::LeftText_DragFloat4("", "##float4", value, window_width * 0.4f))
					{
						Set_Vector4(p_name, value);
						Save();
					}
				}
				else if (info.second.type == Shader::Parameter_Type::Matrix)
				{
					ImGui::Dummy({ 0,0 }); // GUI非対応
				}
				ImGui::PopID();
			}

			if (ImGui::TreeNode(u8"レンダリング設定"))
			{
				static const char* depth[] = { "None", "Less", "Greater", "LEqual", "GEqual", "Equal", "NotEqual", "Always", "None_No_Write", "Less_No_Write", "Greater_No_Write", "LEqual_No_Write", "GEqual_No_Write", "Equal_No_Write", "NotEqual_No_Write", "Always_No_Write" };
				int depth_current = static_cast<int>(pass.depth_stencil_state);
				if (ImGui::LeftText_Combo(u8"深度設定", "##DepthMode", &depth_current, depth, IM_ARRAYSIZE(depth), window_width * 0.6f))
				{
					Set_Depth_Stencil_State(static_cast<DS_State>(depth_current), i);
					Save();
				}

				static const char* cull[] = { "Back", "Front", "None" };
				int cull_current = static_cast<int>(pass.rasterizer_state);
				if (ImGui::LeftText_Combo(u8"カリング", "##Culling", &cull_current, cull, IM_ARRAYSIZE(cull), window_width * 0.6f))
				{
					Set_Rasterizer_State(static_cast<RS_State>(cull_current), i);
					Save();
				}

				static const char* blends[] = { "Off", "Alpha", "Alpha_Test", "Transparent", "Add", "Subtract", "Replace", "Multiply" };
				int blend_current = static_cast<int>(pass.blend_state);
				if (ImGui::LeftText_Combo(u8"ブレンド", "##BlendMode", &blend_current, blends, IM_ARRAYSIZE(blends), window_width * 0.6f))
				{
					Set_Blend_State(static_cast<BS_State>(blend_current), i);
					Save();
				}

				ImGui::LeftText_InputInt(u8"描画キュー", "##Queue", &render_queue, window_width * 0.6f);

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	ImGui::Separator();
	ImGui::Dummy({ 0,0 });
	ImGui::SameLine(window_width - 120.0f);
	if (ImGui::Button(u8"パス追加", { 60,0 }))
	{
		Add_Pass("Shader\\Standard_Shader_VS.hlsl", "Shader\\Standard_Shader_PS.hlsl");
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"パス削除", { 60,0 }))
	{
		Remove_Pass();
	}
}