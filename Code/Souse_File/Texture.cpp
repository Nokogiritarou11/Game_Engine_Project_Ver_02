#include "DxSystem.h"
#include <DirectXTex.h>
#include <Shlwapi.h>
#include <mutex>
#include "Texture.h"
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

Texture::Texture()
{
	ShaderResourceView = nullptr;
	sampler = nullptr;
}

Texture::~Texture()
{
}

bool Texture::Load(string filename, int sampler_state)
{
	HRESULT hr = S_OK;

	//	シェーダーリソースビュー作成
	ComPtr<ID3D11Resource>resource;

	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	static unordered_map<wstring, ComPtr<ID3D11ShaderResourceView>> cache;
	auto it = cache.find(FileName);
	if (it != cache.end())
	{
		//it->second.Attach(*shader_resource_view);
		ShaderResourceView = it->second.Get();
		ShaderResourceView->AddRef();
		ShaderResourceView->GetResource(resource.GetAddressOf());
		Texture_Have = true;
	}
	else
	{
		TexMetadata metadata;
		ScratchImage image;
		u_int texture_flg = 0;

		std::wstring extension = PathFindExtensionW(FileName);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
		//WIC includes several built - in codecs.The following standard codecs are provided with the platform.
		//	Codec																	Mime Types								Decoders	Encoders
		//	BMP(Windows Bitmap Format), BMP Specification v5.						image / bmp								Yes			Yes
		//	GIF(Graphics Interchange Format 89a), GIF Specification 89a / 89m		image / gif								Yes			Yes
		//	ICO(Icon Format)														image / ico								Yes			No
		//	JPEG(Joint Photographic Experts Group), JFIF Specification 1.02			image / jpeg, image / jpe, image / jpg	Yes			Yes
		//	PNG(Portable Network Graphics), PNG Specification 1.2					image / png								Yes			Yes
		//	TIFF(Tagged Image File Format), TIFF Specification 6.0					image / tiff, image / tif				Yes			Yes
		//	Windows Media Photo, HD Photo Specification 1.0							image / vnd.ms - phot					Yes			Yes
		//	DDS(DirectDraw Surface)													image / vnd.ms - dds					Yes			Yes
		if (L".png" == extension || L".jpeg" == extension || L".jpg" == extension || L".jpe" == extension || L".gif" == extension || L".tiff" == extension || L".tif" == extension || L".bmp" == extension)
		{
			hr = LoadFromWICFile(FileName, WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else if (L".dds" == extension)
		{
			hr = LoadFromDDSFile(FileName, DDS_FLAGS::DDS_FLAGS_NONE, &metadata, image);
			texture_flg = D3D11_RESOURCE_MISC_TEXTURECUBE;
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else if (L".tga" == extension || L".vda" == extension || L".icb" == extension || L".vst" == extension)
		{
			hr = LoadFromTGAFile(FileName, &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			_ASSERT_EXPR(false, L"File format not supported.");
		}

		if (hr == S_OK)
		{
			//HRESULT CreateShaderResourceViewEx(
			//	ID3D11Device* pDevice,
			//	const Image* srcImages,
			//	size_t nimages,
			//	const TexMetadata& metadata,
			//	D3D11_USAGE usage,
			//	unsigned int bindFlags,
			//	unsigned int cpuAccessFlags,
			//	unsigned int miscFlags,
			//	bool forceSRGB,
			//	ID3D11ShaderResourceView** ppSRV);
			hr = CreateShaderResourceViewEx(
				DxSystem::Device.Get(),
				image.GetImages(),
				image.GetImageCount(),
				metadata,
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_SHADER_RESOURCE,
				0,
				texture_flg,
				false/*force_srgb*/,
				ShaderResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		ShaderResourceView->GetResource(resource.GetAddressOf());
		cache.insert(make_pair(FileName, ShaderResourceView));
	}

	//テクスチャ情報取得
	ComPtr<ID3D11Texture2D> texture2D;
	resource.Get()->QueryInterface(texture2D.GetAddressOf());
	texture2D.Get()->GetDesc(&texture2d_desc);

	//	サンプラステート作成
	D3D11_SAMPLER_DESC sd = {};
	if (sampler_state == 0)
	{
		sd.Filter = D3D11_FILTER_ANISOTROPIC;		// 異方性フィルタ
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	    // U
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;	    // V
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;	    // W
		sd.MipLODBias = 0;
		sd.MaxAnisotropy = 16;						        // 最大異方性(1Pixelあたりのテクスチャ点数)
		sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
	}
	if (sampler_state == 1)
	{
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.BorderColor[0] = 1.0f;
		sd.BorderColor[1] = 1.0f;
		sd.BorderColor[2] = 1.0f;
		sd.BorderColor[3] = 1.0f;
		sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sd.MaxAnisotropy = 16;
		sd.MipLODBias = 0;
		sd.MinLOD = -FLT_MAX;
		sd.MaxLOD = +FLT_MAX;
	}
	hr = DxSystem::Device->CreateSamplerState(
		&sd, sampler.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Texture_Have = true;
	return true;
}
void Texture::Set(UINT Slot, BOOL flg)
{
	if (flg == FALSE)
	{
		ID3D11ShaderResourceView* rtv[1] = { NULL };
		ID3D11SamplerState* ss[1] = { NULL };
		DxSystem::DeviceContext->PSSetShaderResources(Slot, 1, rtv);
		DxSystem::DeviceContext->PSSetSamplers(Slot, 1, ss);
		DxSystem::DeviceContext->DSSetShaderResources(Slot, 1, rtv);
		DxSystem::DeviceContext->DSSetSamplers(Slot, 1, ss);
		return;
	}
	if (ShaderResourceView)
	{
		DxSystem::DeviceContext->PSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
		DxSystem::DeviceContext->PSSetSamplers(Slot, 1, sampler.GetAddressOf());
		DxSystem::DeviceContext->DSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
		DxSystem::DeviceContext->DSSetSamplers(Slot, 1, sampler.GetAddressOf());
	}
}