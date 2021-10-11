#pragma once
#include <memory>

#include "DxSystem.h"
#include <wrl.h>


namespace BeastEngine
{
	class Material;
	class Shader;
	class Gaussian_Filter;
	class Render_Texture;

	class Shadow_Manager
	{
	public:
		Shadow_Manager();

		void Set_PS_Resource();
		void Set_Shadow_Map_Texture();
		void Set_Default_Shadow_Alpha();
		void Set_Shadow_Map_Texture_Size(u_int size);
		void Filtering_Gaussian();

		u_int Get_Shadow_Map_Texture_Size() { return shadow_map_texture_size; }
		float shadow_bias = 0.01f;
		float shadow_distance = 15;

	private:
		std::unique_ptr<BeastEngine::Render_Texture> render_texture;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_shadow;
		std::shared_ptr<BeastEngine::Material> material_shadow;
		std::unique_ptr<BeastEngine::Gaussian_Filter> gaussian_filter;

		u_int shadow_map_texture_size = 2048;
	};
}