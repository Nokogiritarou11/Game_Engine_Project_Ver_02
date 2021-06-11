#pragma once
#include "DxSystem.h"
#include <vector>
#include <wrl.h>
#include <Original_Math.h>
#include <tchar.h>

class Shadow_Manager
{
public:
	Shadow_Manager();
	~Shadow_Manager();

	void Set_PS_Resource();

	void Set_Shadow_Map();
	void Set_Shadow_Map_Texture_Size(u_int size);
	void Set_Shadow_Distance(float value);
	void Set_Shadow_Bias(float value);

	u_int Get_Shadow_Map_Texture_Size() { return Shadow_Map_Texture_Size; }
	float Get_Shadow_Distance() { return Shadow_Distance; }
	float Get_Shadow_Bias() { return Shadow_Bias; }

private:
	ComPtr<ID3D11DepthStencilView>		DepthStencilView = nullptr;
	ComPtr<ID3D11Texture2D>				DepthStencilTexture = nullptr;
	ComPtr<ID3D11ShaderResourceView>	ShaderResourceView = nullptr;
	ComPtr<ID3D11SamplerState>			sampler = nullptr;

	float Shadow_Bias = 0.01f;
	u_int Shadow_Map_Texture_Size = 1024;
	float Shadow_Distance = 100;
};