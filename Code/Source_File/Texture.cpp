#include <unordered_map>
#include <DirectXTex.h>
#include <Shlwapi.h>
#include <mutex>
#include <clocale>
#include <tchar.h>
#include "DxSystem.h"
#include "Texture.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Misc.h"
using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

shared_ptr<Texture> Texture::Load(const string& texture_path, const u_int& texture_flg)
{
	HRESULT hr = S_OK;

	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, texture_path.c_str(), _TRUNCATE);

	if (const auto it = Engine::asset_manager->cache_texture.find(texture_path); it != Engine::asset_manager->cache_texture.end())
	{
		return it->second;
	}

	ComPtr<ID3D11Resource> resource;
	auto texture = make_shared<Texture>();

	TexMetadata metadata{};
	ScratchImage image;

	std::wstring extension = PathFindExtensionW(FileName);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
	//以下はサンプルからのコピペ
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
		hr = CreateShaderResourceViewEx(
			DxSystem::device.Get(),
			image.GetImages(),
			image.GetImageCount(),
			metadata,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			texture_flg,
			false/*force_srgb*/,
			texture->shader_resource_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	texture->shader_resource_view->GetResource(resource.GetAddressOf());

	//テクスチャ情報取得
	ComPtr<ID3D11Texture2D> texture2D;
	resource.Get()->QueryInterface(texture2D.GetAddressOf());
	texture2D.Get()->GetDesc(&texture->texture2d_desc);

	//	サンプラステート作成
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_ANISOTROPIC;	  // 異方性フィルタ
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // U
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // V
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // W
	sd.MipLODBias = 0;
	sd.MaxAnisotropy = 4; // 最大異方性(1Pixelあたりのテクスチャ点数)
	sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	hr = DxSystem::device->CreateSamplerState(
		&sd, texture->sampler.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Engine::asset_manager->cache_texture.insert(make_pair(texture_path, texture));
	Engine::asset_manager->Registration_Asset(texture);
	texture->path = texture_path;
	return texture;
}

void Texture::Set(UINT Slot, Shader::Shader_Type type)
{
	if (shader_resource_view)
	{
		switch (type)
		{
			case BeastEngine::Shader::Shader_Type::Vertex:
				DxSystem::device_context->VSSetShaderResources(Slot, 1, shader_resource_view.GetAddressOf());
				DxSystem::device_context->VSSetSamplers(Slot, 1, sampler.GetAddressOf());
				break;
			case BeastEngine::Shader::Shader_Type::Geometry:
				DxSystem::device_context->GSSetShaderResources(Slot, 1, shader_resource_view.GetAddressOf());
				DxSystem::device_context->GSSetSamplers(Slot, 1, sampler.GetAddressOf());
				break;
			case BeastEngine::Shader::Shader_Type::Pixel:
				DxSystem::device_context->PSSetShaderResources(Slot, 1, shader_resource_view.GetAddressOf());
				DxSystem::device_context->PSSetSamplers(Slot, 1, sampler.GetAddressOf());
				break;
			case BeastEngine::Shader::Shader_Type::Hull:
				DxSystem::device_context->HSSetShaderResources(Slot, 1, shader_resource_view.GetAddressOf());
				DxSystem::device_context->HSSetSamplers(Slot, 1, sampler.GetAddressOf());
				break;
			case BeastEngine::Shader::Shader_Type::Domain:
				DxSystem::device_context->DSSetShaderResources(Slot, 1, shader_resource_view.GetAddressOf());
				DxSystem::device_context->DSSetSamplers(Slot, 1, sampler.GetAddressOf());
				break;
		}
	}
}