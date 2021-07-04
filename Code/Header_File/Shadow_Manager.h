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
		void Set_Shadow_Distance(float value);
		void Set_Shadow_Bias(float value);

		u_int Get_Shadow_Map_Texture_Size() { return shadow_map_texture_size; }
		float Get_Shadow_Distance() { return shadow_distance; }
		float Get_Shadow_Bias() { return shadow_bias; }

	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				depth_stencil_texture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>			sampler = nullptr;

		float shadow_bias = 0.01f;
		u_int shadow_map_texture_size = 1024;
		float shadow_distance = 100;
	};
}