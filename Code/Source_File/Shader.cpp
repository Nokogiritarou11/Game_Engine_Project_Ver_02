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

	ComPtr<ID3DBlob> BlobError = NULL;
	// コンパイル
	HRESULT hr = D3DCompileFromFile(
		filename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		method,
		shaderModel,
		ShaderFlags,
		0,
		ppBlobOut,
		BlobError.GetAddressOf()
	);

	// エラー出力
	if (BlobError != NULL)
	{
		MessageBoxA(0, (char*)BlobError->GetBufferPointer(), NULL, MB_OK);
	}

	return hr;
}

shared_ptr<Shader> Shader::Create(string shader_path, Shader_Type shader_type)
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
bool Vertex_Shader::Initialize(string filename)
{
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ

	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	hr = Compile(FileName, "main", "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 頂点シェーダ生成
	hr = DxSystem::device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, vs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Reflect shader info
	ID3D11ShaderReflection* pVertexShaderReflection = NULL;
	hr = D3DReflect(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection);
	assert(SUCCEEDED(hr));

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT32 i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

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
	hr = DxSystem::device->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), vertex_layout.GetAddressOf());
	assert(SUCCEEDED(hr));
	//Free allocation shader reflection memory
	pVertexShaderReflection->Release();
	return true;
}

bool Geometry_Shader::Initialize(string filename)
{
	HRESULT hr = S_OK;

	// ジオメトリシェーダ
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(FileName, "main", "gs_5_0", &GSBlob);
	if (FAILED(hr))
	{
		return false;
	}
	hr = DxSystem::device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, gs.GetAddressOf());
	assert(SUCCEEDED(hr));
	return true;
}

bool Pixel_Shader::Initialize(string filename)
{
	HRESULT hr = S_OK;

	// ピクセルシェーダ
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(FileName, "main", "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}
	hr = DxSystem::device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, ps.GetAddressOf());
	assert(SUCCEEDED(hr));
	return true;
}

bool Hull_Shader::Initialize(string filename)
{
	HRESULT hr = S_OK;

	// ハルシェーダ
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	ComPtr<ID3DBlob> HSBlob = NULL;
	hr = Compile(FileName, "main", "hs_5_0", &HSBlob);
	if (FAILED(hr))
	{
		return false;
	}
	hr = DxSystem::device->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, hs.GetAddressOf());
	assert(SUCCEEDED(hr));
	return true;
}

bool Domain_Shader::Initialize(string filename)
{
	HRESULT hr = S_OK;

	// ハルシェーダ
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	ComPtr<ID3DBlob> DSBlob = NULL;
	hr = Compile(FileName, "main", "ds_5_0", &DSBlob);
	if (FAILED(hr))
	{
		return false;
	}
	hr = DxSystem::device->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), NULL, ds.GetAddressOf());
	assert(SUCCEEDED(hr));
	return true;
}

//****************************************************************
//	有効化
void Vertex_Shader::Activate()
{
	DxSystem::device_context->IASetInputLayout(vertex_layout.Get());
	DxSystem::device_context->VSSetShader(vs.Get(), NULL, 0);
}
void Geometry_Shader::Activate()
{
	DxSystem::device_context->GSSetShader(gs.Get(), NULL, 0);
}
void Pixel_Shader::Activate()
{
	DxSystem::device_context->PSSetShader(ps.Get(), NULL, 0);
}
void Hull_Shader::Activate()
{
	DxSystem::device_context->HSSetShader(hs.Get(), NULL, 0);
}
void Domain_Shader::Activate()
{
	DxSystem::device_context->DSSetShader(ds.Get(), NULL, 0);
}