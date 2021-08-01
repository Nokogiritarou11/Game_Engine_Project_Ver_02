#pragma once
#include <InitGUID.h>
#include <dxgidebug.h>
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include <string.h>
#include <assert.h>
#include <DirectXMath.h>
#include "Original_Math.h"

namespace BeastEngine
{
	class Render_Texture
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_target_view;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				texture_render_target;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				texture_depth_stencil;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view_depth;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view_render;

		Render_Texture();
		~Render_Texture() {};

		void Set_Screen_Size(int x, int y);
		void Set_Render_Target();

		int screen_x = 0;
		int screen_y = 0;

		bool Create_Depth_Stencil(int x, int y);
		bool Create_Render_Tartget_View(int x, int y);
		void Clear();

	private:
	};
}