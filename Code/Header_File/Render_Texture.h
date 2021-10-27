#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

namespace BeastEngine
{
	class Render_Texture
	{
	public:
		Render_Texture(const int& x, const int& y, const bool& msaa, const DXGI_FORMAT& format);
		~Render_Texture();

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Get_Texture();
		void Update_Copy_Texture() const;
		Effekseer::Backend::TextureRef& Get_Back_Ground_Texture() { return back_ground_texture; }
		void Set_Screen_Size(const int& x, const int& y);
		void Set_Render_Target();
		void Clear() const;

		int screen_x = 0;
		int screen_y = 0;

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			 texture_render_target;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			 texture_depth_stencil;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			 texture_render_resolve;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			 texture_render_copy;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	 render_target_view;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	 depth_stencil_view;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_render;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_depth;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_resolve;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_copy;

		Effekseer::Backend::TextureRef back_ground_texture;

		bool Create_Depth_Stencil();
		bool Create_Render_Target_View();

		DXGI_FORMAT color_format;
		bool use_msaa;
	};
}