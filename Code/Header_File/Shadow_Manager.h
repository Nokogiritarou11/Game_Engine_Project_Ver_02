#pragma once
#include "DxSystem.h"
#include <vector>
#include <wrl.h>
#include <tchar.h>

namespace BeastEngine
{
	class Shadow_Manager
	{
	public:
		Shadow_Manager();
		~Shadow_Manager();

		void Set_PS_Resource();

		void Set_Shadow_Map_Texture();
		void Set_Shadow_Map_Texture_Size(u_int size);

		u_int Get_Shadow_Map_Texture_Size() { return shadow_map_texture_size; }
		float shadow_bias = 0.0005f;
		float shadow_distance = 15;

	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				depth_stencil_texture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>			sampler = nullptr;

		u_int shadow_map_texture_size = 2048;
	};
}