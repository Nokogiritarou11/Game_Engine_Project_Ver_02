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

		Render_Texture(const int& x, const int& y, const bool& MSAA, const DXGI_FORMAT& format);

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Get_Texture() { return shader_resource_view_render; }
		void Set_Screen_Size(const int& x, const int& y);
		void Set_Render_Target();
		void Clear();

		int screen_x = 0;
		int screen_y = 0;

	private:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_target_view;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				texture_render_target;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				texture_depth_stencil;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view_depth;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view_render;

		bool Create_Depth_Stencil();
		bool Create_Render_Tartget_View();

		DXGI_FORMAT color_format;
		bool use_msaa;
	};
}