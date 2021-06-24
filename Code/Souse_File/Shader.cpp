#include "DxSystem.h"
#include "Shader.h"
#include <locale.h>
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

unordered_map<wstring, Shader::set_of_vertex_shader_and_input_layout> Shader::vertex_cache;
unordered_map<wstring, ID3D11PixelShader*> Shader::pixel_cache;

Shader::~Shader()
{
}

//****************************************************************
//
//
//
//****************************************************************
//------------------------------------------------
//	シェーダー単体コンパイル
//------------------------------------------------
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

//------------------------------------------------
//	シェーダー単体コンパイル
//------------------------------------------------
bool Shader::Create_VS(WCHAR* filename, LPCSTR VSFunc)
{
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ

	auto it = vertex_cache.find(filename);
	if (it != vertex_cache.end())
	{
		vs = it->second.vertex_shader;
		vertex_layout = it->second.input_layout;
	}
	else
	{
		hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
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

		vertex_cache.insert(make_pair(filename, set_of_vertex_shader_and_input_layout(vs.Get(), vertex_layout.Get())));
		// 入力レイアウト設定
		DxSystem::device_context->IASetInputLayout(vertex_layout.Get());
		//Free allocation shader reflection memory
		pVertexShaderReflection->Release();
	}
	return true;
}
bool Shader::Create_PS(WCHAR* filename, LPCSTR PSFunc)
{
	HRESULT hr = S_OK;

	// ピクセルシェーダ
	auto itr = pixel_cache.find(filename);
	if (itr != pixel_cache.end())
	{
		ps = itr->second;
	}
	else
	{
		ComPtr<ID3DBlob> PSBlob = NULL;
		hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
		if (FAILED(hr))
		{
			return false;
		}
		// ピクセルシェーダ生成
		hr = DxSystem::device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, ps.GetAddressOf());
		pixel_cache.insert(make_pair(filename, ps.Get()));
		//PSBlob->Release();
		assert(SUCCEEDED(hr));
	}
	return true;
}

//------------------------------------------------
//	シェーダーセットコンパイル
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc)
{
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ

	auto it = vertex_cache.find(filename);
	if (it != vertex_cache.end())
	{
		vs = it->second.vertex_shader;
		vertex_layout = it->second.input_layout;
	}
	else
	{
		hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
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

		vertex_cache.insert(make_pair(filename, set_of_vertex_shader_and_input_layout(vs.Get(), vertex_layout.Get())));
		// 入力レイアウト設定
		DxSystem::device_context->IASetInputLayout(vertex_layout.Get());
		//Free allocation shader reflection memory
		pVertexShaderReflection->Release();

		/*
		if (UI_Material)
		{
			// 入力レイアウト
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			UINT numElements = ARRAYSIZE(layout);

			// 入力レイアウト生成
			hr = DxSystem::Device->CreateInputLayout(
				layout,
				numElements,
				VSBlob->GetBufferPointer(),
				VSBlob->GetBufferSize(),
				VertexLayout.GetAddressOf()
			);
			assert(SUCCEEDED(hr));
		}
		else
		{
			// 入力レイアウト
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
			UINT numElements = ARRAYSIZE(layout);

			// 入力レイアウト生成
			hr = DxSystem::Device->CreateInputLayout(
				layout,
				numElements,
				VSBlob->GetBufferPointer(),
				VSBlob->GetBufferSize(),
				VertexLayout.GetAddressOf()
			);
			assert(SUCCEEDED(hr));
		}

		vertex_cache.insert(make_pair(filename, set_of_vertex_shader_and_input_layout(VS.Get(), VertexLayout.Get())));

		// 入力レイアウト設定
		DxSystem::DeviceContext->IASetInputLayout(VertexLayout.Get());
		*/
	}

	// ピクセルシェーダ
	auto itr = pixel_cache.find(filename);
	if (itr != pixel_cache.end())
	{
		ps = itr->second;
	}
	else
	{
		ComPtr<ID3DBlob> PSBlob = NULL;
		hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
		if (FAILED(hr))
		{
			return false;
		}
		// ピクセルシェーダ生成
		hr = DxSystem::device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, ps.GetAddressOf());
		pixel_cache.insert(make_pair(filename, ps.Get()));
		//PSBlob->Release();
		assert(SUCCEEDED(hr));
	}

	return true;
}

//------------------------------------------------
//	シェーダーセットコンパイル2
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc)
{
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 頂点シェーダ生成
	hr = DxSystem::device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, vs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT numElements = ARRAYSIZE(layout);

	// 入力レイアウト生成
	hr = DxSystem::device->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		vertex_layout.GetAddressOf()
	);

	assert(SUCCEEDED(hr));

	// 入力レイアウト設定
	DxSystem::device_context->IASetInputLayout(vertex_layout.Get());

	// ピクセルシェーダ
	ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ピクセルシェーダ生成
	hr = DxSystem::device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, ps.GetAddressOf());
	//PSBlob.Get()->Release();
	assert(SUCCEEDED(hr));
	// ジオメトリシェーダ
	ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ジオメトリシェーダ生成
	hr = DxSystem::device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, gs.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

//------------------------------------------------
//	シェーダーセットコンパイル3
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc, LPCSTR HSFunc, LPCSTR DSFunc)
{
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 頂点シェーダ生成
	hr = DxSystem::device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, vs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT numElements = ARRAYSIZE(layout);

	// 入力レイアウト生成
	hr = DxSystem::device->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		vertex_layout.GetAddressOf()
	);

	assert(SUCCEEDED(hr));
	VSBlob.Get()->Release();

	// 入力レイアウト設定
	DxSystem::device_context->IASetInputLayout(vertex_layout.Get());

	// ピクセルシェーダ
	ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ピクセルシェーダ生成
	hr = DxSystem::device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, ps.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ジオメトリシェーダ
	ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));
	// ジオメトリシェーダ生成
	hr = DxSystem::device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, gs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ハルシェーダ
	ComPtr<ID3DBlob> HSBlob = NULL;
	hr = Compile(filename, HSFunc, "hs_5_0", HSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ハルシェーダ生成
	hr = DxSystem::device->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, hs.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ドメインシェーダ
	ComPtr<ID3DBlob> DSBlob = NULL;
	hr = Compile(filename, DSFunc, "ds_5_0", DSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ドメインシェーダ生成
	hr = DxSystem::device->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), NULL, ds.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

//****************************************************************
//
//
//
//****************************************************************
//------------------------------------------------
//	有効化
//------------------------------------------------
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