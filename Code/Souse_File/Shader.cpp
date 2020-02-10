#include "DxSystem.h"
#include <d3dcompiler.h>
#include "Shader.h"
#include <locale.h>
using namespace DirectX;
using namespace std;

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
//	�V�F�[�_�[�P�̃R���p�C��
//------------------------------------------------
HRESULT Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	ComPtr<ID3DBlob> BlobError = NULL;
	// �R���p�C��
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

	// �G���[�o��
	if (BlobError != NULL)
	{
		MessageBoxA(0, (char*)BlobError->GetBufferPointer(), NULL, MB_OK);
	}

	return hr;
}

//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, bool UI_Material)
{
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_

	auto it = vertex_cache.find(filename);
	if (it != vertex_cache.end())
	{
		VS = it->second.vertex_shader;
		VertexLayout = it->second.input_layout;
	}
	else
	{
		hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
		assert(SUCCEEDED(hr));


		// ���_�V�F�[�_����
		hr = DxSystem::Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
		assert(SUCCEEDED(hr));

		/*
		// Reflect shader info
		ID3D11ShaderReflection* pVertexShaderReflection = NULL;
		assert(D3DReflect(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection));

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
		HRESULT hr = DxSystem::Device->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), VertexLayout.GetAddressOf());

		//Free allocation shader reflection memory
		pVertexShaderReflection->Release();
		*/

		if (UI_Material)
		{
			// ���̓��C�A�E�g
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			UINT numElements = ARRAYSIZE(layout);

			// ���̓��C�A�E�g����
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
			// ���̓��C�A�E�g
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
			UINT numElements = ARRAYSIZE(layout);

			// ���̓��C�A�E�g����
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

		// ���̓��C�A�E�g�ݒ�
		DxSystem::DeviceContext->IASetInputLayout(VertexLayout.Get());
	}

	// �s�N�Z���V�F�[�_
	auto itr = pixel_cache.find(filename);
	if (itr != pixel_cache.end())
	{
		PS = itr->second;
		VertexLayout = it->second.input_layout;
	}
	else
	{
		ComPtr<ID3DBlob> PSBlob = NULL;
		hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
		if (FAILED(hr))
		{
			return false;
		}
		// �s�N�Z���V�F�[�_����
		hr = DxSystem::Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
		pixel_cache.insert(make_pair(filename, PS.Get()));
		//PSBlob->Release();
		assert(SUCCEEDED(hr));
	}


	return true;
}


//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��2
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc)
{
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���_�V�F�[�_����
	hr = DxSystem::Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = DxSystem::Device->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		VertexLayout.GetAddressOf()
	);

	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g�ݒ�
	DxSystem::DeviceContext->IASetInputLayout(VertexLayout.Get());

	// �s�N�Z���V�F�[�_
	ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �s�N�Z���V�F�[�_����
	hr = DxSystem::Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
	//PSBlob.Get()->Release();
	assert(SUCCEEDED(hr));
	// �W�I���g���V�F�[�_
	ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �W�I���g���V�F�[�_����
	hr = DxSystem::Device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, GS.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��3
//------------------------------------------------
bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc, LPCSTR HSFunc, LPCSTR DSFunc)
{
	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���_�V�F�[�_����
	hr = DxSystem::Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = DxSystem::Device->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		VertexLayout.GetAddressOf()
	);

	assert(SUCCEEDED(hr));
	VSBlob.Get()->Release();

	// ���̓��C�A�E�g�ݒ�
	DxSystem::DeviceContext->IASetInputLayout(VertexLayout.Get());

	// �s�N�Z���V�F�[�_
	ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �s�N�Z���V�F�[�_����
	hr = DxSystem::Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �W�I���g���V�F�[�_
	ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));
	// �W�I���g���V�F�[�_����
	hr = DxSystem::Device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, GS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �n���V�F�[�_
	ComPtr<ID3DBlob> HSBlob = NULL;
	hr = Compile(filename, HSFunc, "hs_5_0", HSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �n���V�F�[�_����
	hr = DxSystem::Device->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, HS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �h���C���V�F�[�_
	ComPtr<ID3DBlob> DSBlob = NULL;
	hr = Compile(filename, DSFunc, "ds_5_0", DSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �h���C���V�F�[�_����
	hr = DxSystem::Device->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), NULL, DS.GetAddressOf());
	assert(SUCCEEDED(hr));



	return true;
}

//****************************************************************
//
//
//
//****************************************************************
//------------------------------------------------
//	�L����
//------------------------------------------------
void Shader::Activate()
{
	DxSystem::DeviceContext->VSSetShader(VS.Get(), NULL, 0);
	DxSystem::DeviceContext->PSSetShader(PS.Get(), NULL, 0);
	DxSystem::DeviceContext->GSSetShader(GS.Get(), NULL, 0);
	DxSystem::DeviceContext->HSSetShader(HS.Get(), NULL, 0);
	DxSystem::DeviceContext->DSSetShader(DS.Get(), NULL, 0);

}
