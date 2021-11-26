#include <clocale>
#include "DxSystem.h"
#include "Shader.h"
#include "Engine.h"
#include "Asset_Manager.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

//	シェーダーコンパイル
HRESULT Shader::Compile(const WCHAR* filename, const LPCSTR method, const LPCSTR shader_model, ID3DBlob** pp_blob_out)
{
	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	ComPtr<ID3DBlob> BlobError = nullptr;
	// コンパイル
	const HRESULT hr = D3DCompileFromFile(
		filename,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		method,
		shader_model,
		ShaderFlags,
		0,
		pp_blob_out,
		BlobError.GetAddressOf()
	);

	// エラー出力
	if (BlobError != nullptr)
	{
		MessageBoxA(nullptr, static_cast<char*>(BlobError->GetBufferPointer()), nullptr, MB_OK);
	}

	return hr;
}

void Shader::Reflect_Resource_Buffer(const ComPtr<ID3D11ShaderReflection>& reflector)
{
	D3D11_SHADER_DESC shaderdesc;
	reflector->GetDesc(&shaderdesc);

	// リフレクション
	for (size_t i = 0; i < shaderdesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC desc;
		HRESULT hr = reflector->GetResourceBindingDesc(i, &desc);
		assert(SUCCEEDED(hr));

		if (desc.Type == D3D10_SHADER_INPUT_TYPE::D3D10_SIT_CBUFFER)
		{
			// コンスタントバッファ
			// 0番はエンジン予約バッファなので飛ばす
			if (desc.BindPoint != 0)
			{
				auto cb = reflector->GetConstantBufferByName(desc.Name);
				D3D11_SHADER_BUFFER_DESC b_desc;
				cb->GetDesc(&b_desc);

				ConstantBuffer_Info info;
				info.name = desc.Name;
				info.register_number = desc.BindPoint;
				info.byte_size = b_desc.Size;

				for (size_t j = 0; j < b_desc.Variables; ++j)
				{
					auto v = cb->GetVariableByIndex(j);
					D3D11_SHADER_VARIABLE_DESC v_desc;
					v->GetDesc(&v_desc);
					auto t = v->GetType();
					D3D11_SHADER_TYPE_DESC t_desc;
					t->GetDesc(&t_desc);

					//パラメータの型ごとの処理
					auto type = Parameter_Type::Int;
					if (t_desc.Class == D3D10_SVC_SCALAR)
					{
						if (t_desc.Type == D3D10_SVT_INT || t_desc.Type == D3D10_SVT_UINT)
						{
							type = Parameter_Type::Int;
						}
						else if (t_desc.Type == D3D10_SVT_FLOAT)
						{
							type = Parameter_Type::Float;
						}
					}
					else if (t_desc.Class == D3D10_SVC_VECTOR)
					{
						if (v_desc.Size == 8)
						{
							type = Parameter_Type::Vector2;
						}
						else if (v_desc.Size == 12)
						{
							type = Parameter_Type::Vector3;
						}
						else if (v_desc.Size == 16)
						{
							type = Parameter_Type::Vector4;
						}
					}
					else if (t_desc.Class == D3D10_SVC_MATRIX_ROWS || t_desc.Class == D3D10_SVC_MATRIX_COLUMNS)
					{
						type = Parameter_Type::Matrix;
					}
					else
					{
						assert(false);
					}

					vector<std::byte> default_value(v_desc.Size);
					memcpy(&default_value[0], v_desc.DefaultValue, v_desc.Size);

					Parameter_Info p_info = { v_desc.Name, type, default_value, v_desc.Size, v_desc.StartOffset };
					info.parameters.emplace_back(p_info);
				}
				constant_buffer_info.emplace_back(info);
			}
		}
		else if (desc.Type == D3D10_SIT_TEXTURE)
		{
			// テクスチャ
			// 0番はエンジン予約(シャドウテクスチャ)なので飛ばす
			if (desc.BindPoint != 0)
			{
				Texture_Info info = { desc.Name, desc.BindPoint };
				texture_info.emplace_back(info);
			}
		}
	}
}

shared_ptr<Shader> Shader::Create(const string& shader_path, const Shader_Type& shader_type)
{
	//キャッシュがあれば返す
	if (const auto it = Engine::asset_manager->cache_shader.find(shader_path); it != Engine::asset_manager->cache_shader.end())
	{
		return it->second;
	}

	//新規作成
	shared_ptr<Shader> shader;
	switch (shader_type)
	{
		case Shader_Type::Vertex:
			shader = make_shared<Vertex_Shader>();
			break;
		case Shader_Type::Geometry:
			shader = make_shared<Geometry_Shader>();
			break;
		case Shader_Type::Pixel:
			shader = make_shared<Pixel_Shader>();
			break;
		case Shader_Type::Hull:
			shader = make_shared<Hull_Shader>();
			break;
		case Shader_Type::Domain:
			shader = make_shared<Domain_Shader>();
			break;
	}
	shader->Initialize(shader_path);
	Engine::asset_manager->cache_shader.insert(make_pair(shader_path, shader));
	return shader;
}

bool Vertex_Shader::Initialize(const string& filename)
{
	HRESULT hr = S_OK;

	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	// 頂点シェーダ
	ComPtr<ID3DBlob> blob = nullptr;

	hr = Compile(FileName, "main", "vs_5_0", blob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 頂点シェーダ生成
	hr = DxSystem::device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, vs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// リフレクション
	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&reflector));
	assert(SUCCEEDED(hr));

	Reflect_Resource_Buffer(reflector);
	Reflect_InputLayout(reflector, blob);

	//開放
	reflector->Release();
	return true;
}

void Vertex_Shader::Reflect_InputLayout(const ComPtr<ID3D11ShaderReflection>& reflector, const ComPtr<ID3DBlob>& blob)
{
	// シェーダーの情報を得る
	D3D11_SHADER_DESC shader_desc;
	reflector->GetDesc(&shader_desc);

	// シェーダ情報からInputLayout情報を得る
	std::vector<D3D11_INPUT_ELEMENT_DESC> input_layout_desc;
	for (UINT32 i = 0; i < shader_desc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC param_desc;
		reflector->GetInputParameterDesc(i, &param_desc);

		D3D11_INPUT_ELEMENT_DESC element_desc;
		element_desc.SemanticName = param_desc.SemanticName;
		element_desc.SemanticIndex = param_desc.SemanticIndex;
		element_desc.InputSlot = 0;
		element_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		element_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		element_desc.InstanceDataStepRate = 0;

		if (param_desc.SemanticName != "SV_VertexID")
		{
			if (param_desc.Mask == 1)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (param_desc.Mask <= 3)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (param_desc.Mask <= 7)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (param_desc.Mask <= 15)
			{
				if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) element_desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) element_desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) element_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			// 得られた情報からレイアウトに追加する
			input_layout_desc.push_back(element_desc);
		}
	}

	if (!input_layout_desc.empty())
	{
		// InputLayoutの作成
		const HRESULT hr = DxSystem::device->CreateInputLayout(&input_layout_desc[0], input_layout_desc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), vertex_layout.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
}

bool Geometry_Shader::Initialize(const string& filename)
{
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	// ジオメトリシェーダ
	ComPtr<ID3DBlob> blob = nullptr;

	HRESULT hr = Compile(FileName, "main", "gs_5_0", &blob);
	assert(SUCCEEDED(hr));

	hr = DxSystem::device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, gs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// リフレクション
	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&reflector));
	assert(SUCCEEDED(hr));

	Reflect_Resource_Buffer(reflector);

	//開放
	reflector->Release();
	return true;
}

bool Pixel_Shader::Initialize(const string& filename)
{
	HRESULT hr = S_OK;

	setlocale(LC_ALL, "japanese");
	wchar_t w_file_name[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, w_file_name, MAX_PATH, filename.c_str(), _TRUNCATE);

	// ピクセルシェーダ
	ComPtr<ID3DBlob> blob = nullptr;

	hr = Compile(w_file_name, "main", "ps_5_0", &blob);
	assert(SUCCEEDED(hr));

	hr = DxSystem::device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ps.GetAddressOf());
	assert(SUCCEEDED(hr));

	// リフレクション
	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&reflector));
	assert(SUCCEEDED(hr));

	Reflect_Resource_Buffer(reflector);

	//開放
	reflector->Release();
	return true;
}

bool Hull_Shader::Initialize(const string& filename)
{
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	// ハルシェーダ
	ComPtr<ID3DBlob> blob = nullptr;

	HRESULT hr = Compile(FileName, "main", "hs_5_0", &blob);
	assert(SUCCEEDED(hr));

	hr = DxSystem::device->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, hs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// リフレクション
	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&reflector));
	assert(SUCCEEDED(hr));

	Reflect_Resource_Buffer(reflector);

	//開放
	reflector->Release();
	return true;
}

bool Domain_Shader::Initialize(const string& filename)
{
	HRESULT hr = S_OK;

	setlocale(LC_ALL, "japanese");
	wchar_t w_file_name[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, w_file_name, MAX_PATH, filename.c_str(), _TRUNCATE);

	// ハルシェーダ
	ComPtr<ID3DBlob> blob = nullptr;

	hr = Compile(w_file_name, "main", "ds_5_0", &blob);
	assert(SUCCEEDED(hr));

	hr = DxSystem::device->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ds.GetAddressOf());
	assert(SUCCEEDED(hr));

	// リフレクション
	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&reflector));
	assert(SUCCEEDED(hr));

	Reflect_Resource_Buffer(reflector);

	//開放
	reflector->Release();
	return true;
}

void Vertex_Shader::Activate()
{
	DxSystem::device_context->IASetInputLayout(vertex_layout.Get());
	DxSystem::device_context->VSSetShader(vs.Get(), nullptr, 0);
}
void Geometry_Shader::Activate()
{
	DxSystem::device_context->GSSetShader(gs.Get(), nullptr, 0);
}
void Pixel_Shader::Activate()
{
	DxSystem::device_context->PSSetShader(ps.Get(), nullptr, 0);
}
void Hull_Shader::Activate()
{
	DxSystem::device_context->HSSetShader(hs.Get(), nullptr, 0);
}
void Domain_Shader::Activate()
{
	DxSystem::device_context->DSSetShader(ds.Get(), nullptr, 0);
}