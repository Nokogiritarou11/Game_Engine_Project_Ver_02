#include <locale.h>
#include "DxSystem.h"
#include "Shader.h"
#include "Engine.h"
#include "Asset_Manager.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

//	シェーダーコンパイル
HRESULT Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	ComPtr<ID3DBlob> BlobError = nullptr;
	// コンパイル
	HRESULT hr = D3DCompileFromFile(
		filename,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		method,
		shaderModel,
		ShaderFlags,
		0,
		ppBlobOut,
		BlobError.GetAddressOf()
	);

	// エラー出力
	if (BlobError != nullptr)
	{
		MessageBoxA(0, (char*)BlobError->GetBufferPointer(), nullptr, MB_OK);
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
				D3D11_SHADER_BUFFER_DESC bdesc;
				cb->GetDesc(&bdesc);

				ConstantBuffer_Info info;
				info.name = desc.Name;
				info.register_number = desc.BindPoint;
				info.byte_size = bdesc.Size;

				for (size_t j = 0; j < bdesc.Variables; ++j)
				{
					auto v = cb->GetVariableByIndex(j);
					D3D11_SHADER_VARIABLE_DESC vdesc;
					v->GetDesc(&vdesc);
					auto t = v->GetType();
					D3D11_SHADER_TYPE_DESC tdesc;
					t->GetDesc(&tdesc);

					Parameter_Type type;
					if (tdesc.Class == D3D10_SVC_SCALAR)
					{
						if (tdesc.Type == D3D10_SVT_INT || tdesc.Type == D3D10_SVT_UINT) { type = Parameter_Type::INT; }
						else if (tdesc.Type == D3D10_SVT_FLOAT) { type = Parameter_Type::FLOAT; }
					}
					else if (tdesc.Class == D3D10_SVC_VECTOR)
					{
						if (vdesc.Size == 8) { type = Parameter_Type::VECTOR2; }
						else if (vdesc.Size == 12) { type = Parameter_Type::VECTOR3; }
						else if (vdesc.Size == 16) { type = Parameter_Type::VECTOR4; }
					}
					else if (tdesc.Class == D3D10_SVC_MATRIX_ROWS || tdesc.Class == D3D10_SVC_MATRIX_COLUMNS)
					{
						type = Parameter_Type::MATRIX;
					}
					else
					{
						assert(false);
					}

					Parameter_Info p_info = { vdesc.Name, type, vdesc.Size, vdesc.StartOffset };
					info.parameters.emplace_back(p_info);
				}
				constant_buffer_info.emplace_back(info);
			}
		}
		else if (desc.Type == D3D10_SHADER_INPUT_TYPE::D3D10_SIT_TEXTURE)
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
	auto it = Engine::asset_manager->cache_shader.find(shader_path);
	if (it != Engine::asset_manager->cache_shader.end())
	{
		return it->second;
	}
	else
	{
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

	return nullptr;
}

//	シェーダー単体コンパイル
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
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);
	assert(SUCCEEDED(hr));

	Reflect_Resource_Buffer(reflector);
	Reflect_InputLayout(reflector, blob);

	//開放
	reflector->Release();
	return true;
}

void Vertex_Shader::Reflect_InputLayout(const ComPtr<ID3D11ShaderReflection>& reflector, ComPtr<ID3DBlob>& blob)
{
	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	reflector->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT32 i = 0; i < shaderDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflector->GetInputParameterDesc(i, &paramDesc);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}

	// Try to create Input Layout
	HRESULT hr = DxSystem::device->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), vertex_layout.GetAddressOf());
	assert(SUCCEEDED(hr));
}

bool Geometry_Shader::Initialize(const string& filename)
{
	HRESULT hr = S_OK;

	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	// ジオメトリシェーダ
	ComPtr<ID3DBlob> blob = nullptr;

	hr = Compile(FileName, "main", "gs_5_0", &blob);
	assert(SUCCEEDED(hr));

	hr = DxSystem::device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, gs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// リフレクション
	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);
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
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	// ピクセルシェーダ
	ComPtr<ID3DBlob> blob = nullptr;

	hr = Compile(FileName, "main", "ps_5_0", &blob);
	assert(SUCCEEDED(hr));

	hr = DxSystem::device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ps.GetAddressOf());
	assert(SUCCEEDED(hr));

	// リフレクション
	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);
	assert(SUCCEEDED(hr));

	Reflect_Resource_Buffer(reflector);

	//開放
	reflector->Release();
	return true;
}

bool Hull_Shader::Initialize(const string& filename)
{
	HRESULT hr = S_OK;

	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	// ハルシェーダ
	ComPtr<ID3DBlob> blob = nullptr;

	hr = Compile(FileName, "main", "hs_5_0", &blob);
	assert(SUCCEEDED(hr));

	hr = DxSystem::device->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, hs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// リフレクション
	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);
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
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	// ハルシェーダ
	ComPtr<ID3DBlob> blob = nullptr;

	hr = Compile(FileName, "main", "ds_5_0", &blob);
	assert(SUCCEEDED(hr));

	hr = DxSystem::device->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, ds.GetAddressOf());
	assert(SUCCEEDED(hr));

	// リフレクション
	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);
	assert(SUCCEEDED(hr));

	Reflect_Resource_Buffer(reflector);

	//開放
	reflector->Release();
	return true;
}

//****************************************************************
//	有効化
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