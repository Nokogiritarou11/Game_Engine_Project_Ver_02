#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <wrl.h>
#include <string>
#include <clocale>
#include <tchar.h>
using Microsoft::WRL::ComPtr;

class Texture
{
protected:
	ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	ComPtr<ID3D11SamplerState>       sampler            = nullptr;
	ComPtr<ID3D11RenderTargetView>   RenderTargetView   = nullptr;

	// テクスチャ情報
	D3D11_TEXTURE2D_DESC texture2d_desc;

public:
	Texture();
	virtual ~Texture();
	bool Load(std::string filename);
	void Set(UINT Slot = 0, BOOL flg = TRUE);
	UINT GetWidth() { return texture2d_desc.Width; }
	UINT GetHeight() { return texture2d_desc.Height; }
	ID3D11RenderTargetView* GetRenderTarget() { return RenderTargetView.Get(); }
};
