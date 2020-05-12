#include "DxSystem.h"
#include "WICTextureLoader.h"
#include "Texture.h"
using namespace std;

Texture::Texture()
{
	ShaderResourceView = nullptr;
	sampler = nullptr;
}

Texture::~Texture()
{
}

bool Texture::Load(const wchar_t* filename)
{
	HRESULT hr = S_OK;

	//	シェーダーリソースビュー作成
	ComPtr<ID3D11Resource>resource;

	static unordered_map<wstring, ComPtr<ID3D11ShaderResourceView>> cache;
	auto it = cache.find(filename);
	if (it != cache.end())
	{
		//it->second.Attach(*shader_resource_view);
		ShaderResourceView = it->second.Get();
		ShaderResourceView->AddRef();
		ShaderResourceView->GetResource(resource.GetAddressOf());
	}
	else
	{
		hr = DirectX::CreateWICTextureFromFile(DxSystem::Device.Get(),
			filename, resource.GetAddressOf(), ShaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			assert(SUCCEEDED(hr));
			return false;
		}
		cache.insert(make_pair(filename, ShaderResourceView));
	}

	//テクスチャ情報取得
	ComPtr<ID3D11Texture2D> texture2D;
	resource.Get()->QueryInterface(texture2D.GetAddressOf());
	texture2D.Get()->GetDesc(&texture2d_desc);

	//	サンプラステート作成
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter             = D3D11_FILTER_ANISOTROPIC;		// 異方性フィルタ
	sd.AddressU           = D3D11_TEXTURE_ADDRESS_WRAP;	    // U
	sd.AddressV           = D3D11_TEXTURE_ADDRESS_WRAP;	    // V
	sd.AddressW           = D3D11_TEXTURE_ADDRESS_WRAP;	    // W
	sd.MipLODBias         = 0;
	sd.MaxAnisotropy      = 16;						        // 最大異方性(1Pixelあたりのテクスチャ点数)
	sd.ComparisonFunc     = D3D11_COMPARISON_ALWAYS;
	sd.MinLOD             = 0;
	sd.MaxLOD             = D3D11_FLOAT32_MAX;

	hr = DxSystem::Device->CreateSamplerState(
		&sd, sampler.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}
void Texture::Set(UINT Slot, BOOL flg)
{
	if (flg == FALSE) {
		ID3D11ShaderResourceView* rtv[1] = { NULL };
		ID3D11SamplerState* ss[1] = { NULL };
		DxSystem::DeviceContext->PSSetShaderResources(Slot, 1, rtv);
		DxSystem::DeviceContext->PSSetSamplers(Slot, 1, ss);
		DxSystem::DeviceContext->DSSetShaderResources(Slot, 1, rtv);
		DxSystem::DeviceContext->DSSetSamplers(Slot, 1, ss);
		return;
	}
	if (ShaderResourceView) {
		DxSystem::DeviceContext->PSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
		DxSystem::DeviceContext->PSSetSamplers(Slot, 1, sampler.GetAddressOf());
		DxSystem::DeviceContext->DSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
		DxSystem::DeviceContext->DSSetSamplers(Slot, 1, sampler.GetAddressOf());
	}
}

bool Texture::Create(u_int width, u_int height, DXGI_FORMAT format)
{
	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width            = width;
	texture2d_desc.Height           = height;
	texture2d_desc.MipLevels        = 1;
	texture2d_desc.ArraySize        = 1;
	texture2d_desc.Format           = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage            = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags   = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = DxSystem::Device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format             = format;
	rtvd.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = DxSystem::Device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format                    = format;
	srvd.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels       = 1;
	hr = DxSystem::Device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//	サンプラステート作成
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU       = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressV       = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW       = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//ボーダーカラー
	sd.BorderColor[0] = 1.0f;
	sd.BorderColor[1] = 1.0f;
	sd.BorderColor[2] = 1.0f;
	sd.BorderColor[3] = 1.0f;

	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	hr = DxSystem::Device->CreateSamplerState(&sd, sampler.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}