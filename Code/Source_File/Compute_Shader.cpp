#include <locale.h>
#include "DxSystem.h"
#include "Compute_Shader.h"
#include "Engine.h"
#include "Asset_Manager.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

shared_ptr<Compute_Shader> Compute_Shader::Create(string shader_path)
{
	shared_ptr<Compute_Shader> shader = make_shared<Compute_Shader>();

	auto it = Engine::asset_manager->cache_compute_shader.find(shader_path);
	if (it != Engine::asset_manager->cache_compute_shader.end())
	{
		shader->cs = it->second;
	}
	else
	{
		setlocale(LC_ALL, "japanese");
		wchar_t FileName[MAX_PATH] = { 0 };
		size_t ret = 0;
		mbstowcs_s(&ret, FileName, MAX_PATH, shader_path.c_str(), _TRUNCATE);

		HRESULT hr = S_OK;
		ComPtr<ID3DBlob> CSBlob = NULL;
		hr = shader->Compile(FileName, "main", "cs_5_0", &CSBlob);
		assert(SUCCEEDED(hr));
		hr = DxSystem::device->CreateComputeShader(CSBlob->GetBufferPointer(), CSBlob->GetBufferSize(), NULL, shader->cs.GetAddressOf());
		assert(SUCCEEDED(hr));

		Engine::asset_manager->cache_compute_shader.insert(make_pair(shader_path, shader->cs));
	}
	return shader;
}

//	シェーダーコンパイル
HRESULT Compute_Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
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

void Compute_Shader::Create_Buffer_Input(UINT size, UINT count, void* init_data)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = size * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = size;

	if (init_data)
	{
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = init_data;

		hr = DxSystem::device->CreateBuffer(&desc, &initData, buffer_input.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	else
	{
		hr = DxSystem::device->CreateBuffer(&desc, nullptr, buffer_input.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = count;

	hr = DxSystem::device->CreateShaderResourceView(buffer_input.Get(), &srvDesc, srv_input.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void Compute_Shader::Create_Buffer_Result(UINT size, UINT count, void* init_data)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = size * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = size;

	if (init_data)
	{
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = init_data;

		hr = DxSystem::device->CreateBuffer(&desc, &initData, buffer_result.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	else
	{
		hr = DxSystem::device->CreateBuffer(&desc, nullptr, buffer_result.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	memset(&uavDesc, 0, sizeof(uavDesc));

	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.NumElements = count;

	hr = DxSystem::device->CreateUnorderedAccessView(buffer_result.Get(), &uavDesc, uav_result.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void Compute_Shader::Run()
{
	//　コンピュートシェーダー実行
	DxSystem::device_context->CSSetShader(cs.Get(), NULL, 0);
	DxSystem::device_context->CSSetShaderResources(0, 1, srv_input.GetAddressOf());
	DxSystem::device_context->CSSetUnorderedAccessViews(0, 1, uav_result.GetAddressOf(), 0);
	DxSystem::device_context->Dispatch(128, 1, 1);
}

ComPtr<ID3D11Buffer> Compute_Shader::Get_Copy_Buffer()
{
	ComPtr<ID3D11Buffer> clone;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	buffer_result->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(DxSystem::device->CreateBuffer(&desc, nullptr, &clone)))
	{
		DxSystem::device_context->CopyResource(clone.Get(), buffer_result.Get());
	}

	return clone;
}