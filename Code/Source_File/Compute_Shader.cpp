#include <clocale>
#include "DxSystem.h"
#include "Compute_Shader.h"
#include "Engine.h"
#include "Asset_Manager.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

shared_ptr<Compute_Shader> Compute_Shader::Create(const string& shader_path)
{
	auto shader = make_shared<Compute_Shader>();

	//キャッシュがある場合はそれを返す
	if (const auto it = Engine::asset_manager->cache_compute_shader.find(shader_path); it != Engine::asset_manager->cache_compute_shader.end())
	{
		shader->cs = it->second;
	}
	else
	{
		//コンパイルしてキャッシュ
		setlocale(LC_ALL, "japanese");
		wchar_t file_name[MAX_PATH] = { 0 };
		size_t ret = 0;
		mbstowcs_s(&ret, file_name, MAX_PATH, shader_path.c_str(), _TRUNCATE);

		ComPtr<ID3DBlob> cs_blob = nullptr;
		HRESULT hr = shader->Compile(file_name, "main", "cs_5_0", &cs_blob);
		assert(SUCCEEDED(hr));
		hr = DxSystem::device->CreateComputeShader(cs_blob->GetBufferPointer(), cs_blob->GetBufferSize(), nullptr, shader->cs.GetAddressOf());
		assert(SUCCEEDED(hr));

		Engine::asset_manager->cache_compute_shader.insert(make_pair(shader_path, shader->cs));
	}
	return shader;
}

HRESULT Compute_Shader::Compile(const WCHAR* filename, const LPCSTR method, const LPCSTR shader_model, ID3DBlob** pp_blob_out)
{
	DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
	shader_flags |= D3DCOMPILE_DEBUG;
	shader_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	ComPtr<ID3DBlob> blob_error = nullptr;
	// コンパイル
	const HRESULT hr = D3DCompileFromFile(
		filename,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		method,
		shader_model,
		shader_flags,
		0,
		pp_blob_out,
		blob_error.GetAddressOf()
	);

	// エラー出力
	if (blob_error != nullptr)
	{
		MessageBoxA(nullptr, static_cast<char*>(blob_error->GetBufferPointer()), nullptr, MB_OK);
	}

	return hr;
}

void Compute_Shader::Create_Buffer_Input(UINT size, UINT count, void* init_data)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC desc = {};

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = size * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = size;

	if (init_data)
	{
		//初期データのコピー
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

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srv_desc.BufferEx.FirstElement = 0;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.BufferEx.NumElements = count;

	hr = DxSystem::device->CreateShaderResourceView(buffer_input.Get(), &srv_desc, srv_input.GetAddressOf());
	assert(SUCCEEDED(hr));

	//計算量をなるべく減らすために要素数に一番近い値を算出する
	UINT c = 1;
	while (true)
	{
		if (count <= c * 64)
		{
			contents_count = c;
			break;
		}
		c *= 2;
	}
}

void Compute_Shader::Create_Buffer_Result(UINT size, UINT count, void* init_data)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC desc = {};

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = size * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = size;

	if (init_data)
	{
		//初期データのコピー
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

	//UAV
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};

	uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Format = DXGI_FORMAT_UNKNOWN;
	uav_desc.Buffer.NumElements = count;

	hr = DxSystem::device->CreateUnorderedAccessView(buffer_result.Get(), &uav_desc, uav_result.GetAddressOf());
	assert(SUCCEEDED(hr));

	//SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srv_desc.BufferEx.FirstElement = 0;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.BufferEx.NumElements = count;

	hr = DxSystem::device->CreateShaderResourceView(buffer_result.Get(), &srv_desc, srv_result.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void Compute_Shader::Run()
{
	// コンピュートシェーダー実行
	DxSystem::device_context->CSSetShader(cs.Get(), nullptr, 0);
	DxSystem::device_context->CSSetShaderResources(0, 1, srv_input.GetAddressOf());
	DxSystem::device_context->CSSetUnorderedAccessViews(0, 1, uav_result.GetAddressOf(), nullptr);

	DxSystem::device_context->Dispatch(contents_count, 1, 1);

	constexpr ID3D11UnorderedAccessView* const pUAV[1] = { nullptr };
	DxSystem::device_context->CSSetUnorderedAccessViews(0, 1, pUAV, nullptr);
}

ComPtr<ID3D11Buffer> Compute_Shader::Get_Copy_Buffer() const
{
	ComPtr<ID3D11Buffer> clone;
	D3D11_BUFFER_DESC desc = {};

	buffer_result->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	//計算済みのリソースをコピーする
	if (SUCCEEDED(DxSystem::device->CreateBuffer(&desc, nullptr, &clone)))
	{
		DxSystem::device_context->CopyResource(clone.Get(), buffer_result.Get());
	}

	return clone;
}
