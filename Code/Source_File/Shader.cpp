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

shared_ptr<Shader> Shader::Create(string VSName, string PSName)
{
	if (VSName == "" && PSName == "")
	{
		return nullptr;
	}
	else
	{
		string hash = VSName + "*" + PSName;

		auto it = Engine::asset_manager->cache_shader.find(hash);
		if (it != Engine::asset_manager->cache_shader.end())
		{
			return it->second;
		}
		else
		{
			shared_ptr<Shader> shader = make_shared<Shader>();
			if (VSName != "")
			{
				shader->Create_VS(VSName);
			}
			if (PSName != "")
			{
				shader->Create_PS(PSName);
			}
			Engine::asset_manager->cache_shader.insert(make_pair(hash, shader));
			return shader;
		}
	}

	return nullptr;
}

//	シェーダー単体コンパイル
bool Shader::Create_VS(string filename)
{
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ

	auto it = Engine::asset_manager->cache_vertex.find(filename);
	if (it != Engine::asset_manager->cache_vertex.end())
	{
		vs = it->second.first;
		vertex_layout = it->second.second;
	}
	else
	{
		setlocale(LC_ALL, "japanese");
		wchar_t FileName[MAX_PATH] = { 0 };
		size_t ret = 0;
		mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

		hr = Compile(FileName, "VSMain", "vs_5_0", VSBlob.GetAddressOf());
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

		Engine::asset_manager->cache_vertex.insert(make_pair(filename, make_pair(vs.Get(), vertex_layout.Get())));
		// 入力レイアウト設定
		DxSystem::device_context->IASetInputLayout(vertex_layout.Get());
		//Free allocation shader reflection memory
		pVertexShaderReflection->Release();
	}
	return true;
}
bool Shader::Create_PS(string filename)
{
	HRESULT hr = S_OK;

	// ピクセルシェーダ
	auto itr = Engine::asset_manager->cache_pixel.find(filename);
	if (itr != Engine::asset_manager->cache_pixel.end())
	{
		ps = itr->second;
	}
	else
	{
		setlocale(LC_ALL, "japanese");
		wchar_t FileName[MAX_PATH] = { 0 };
		size_t ret = 0;
		mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

		ComPtr<ID3DBlob> PSBlob = NULL;
		hr = Compile(FileName, "PSMain", "ps_5_0", &PSBlob);
		if (FAILED(hr))
		{
			return false;
		}
		// ピクセルシェーダ生成
		hr = DxSystem::device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, ps.GetAddressOf());
		Engine::asset_manager->cache_pixel.insert(make_pair(filename, ps.Get()));
		//PSBlob->Release();
		assert(SUCCEEDED(hr));
	}
	return true;
}

//****************************************************************

//	有効化
void Shader::Activate()
{
	DxSystem::device_context->VSSetShader(vs.Get(), NULL, 0);
	DxSystem::device_context->PSSetShader(ps.Get(), NULL, 0);
	DxSystem::device_context->GSSetShader(gs.Get(), NULL, 0);
	DxSystem::device_context->HSSetShader(hs.Get(), NULL, 0);
	DxSystem::device_context->DSSetShader(ds.Get(), NULL, 0);
}

void Shader::Activate_VS()
{
	DxSystem::device_context->VSSetShader(vs.Get(), NULL, 0);
}
void Shader::Activate_PS()
{
	DxSystem::device_context->PSSetShader(ps.Get(), NULL, 0);
}
void Shader::Activate_GS()
{
	DxSystem::device_context->GSSetShader(gs.Get(), NULL, 0);
}
void Shader::Activate_HS()
{
	DxSystem::device_context->HSSetShader(hs.Get(), NULL, 0);
}
void Shader::Activate_DS()
{
	DxSystem::device_context->DSSetShader(ds.Get(), NULL, 0);
}