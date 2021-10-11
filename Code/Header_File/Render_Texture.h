#pragma once
#include <d3d11.h>
#include <wrl.h>

namespace BeastEngine
{
	class Render_Texture
	{
	public:

		Render_Texture(const int& x, const int& y, const bool& msaa, const DXGI_FORMAT& format);

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Get_Texture();
		void Set_Screen_Size(const int& x, const int& y);
		void Set_Render_Target();
		void Clear() const;

		int screen_x = 0;
		int screen_y = 0;

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				texture_render_target;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				texture_depth_stencil;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				texture_render_resolve;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		render_target_view;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view_render;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view_depth;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view_resolve;

		bool Create_Depth_Stencil();
		bool Create_Render_Target_View();

		DXGI_FORMAT color_format;
		bool use_msaa;
	};
}