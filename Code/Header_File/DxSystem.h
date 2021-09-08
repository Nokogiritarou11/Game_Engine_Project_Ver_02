#pragma once
#include <InitGUID.h>
#include <dxgidebug.h>
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include <Windows.h>
#include <string.h>
#include <DirectXMath.h>
#include "Misc.h"
#include <assert.h>
#define SAFE_RELEASE(x) if((x)){(x)->Release();(x)=NULL;}
#define SAFE_DELETE(x) if((x)){delete (x);(x)=NULL;}
#define SAFE_DELETE_ARRAY(x) if(x){delete[] x; x=0;}

#define Debug_Log( str, ... ) \
      { \
		wchar_t buffer[256]; \
		wsprintfW(buffer, str, __VA_ARGS__); \
		OutputDebugString(buffer); \
      }

namespace BeastEngine
{
	//DepthStencilState
	enum class DS_State { None, Less, Greater, LEqual, GEqual, Equal, NotEqual, Always, None_No_Write, Less_No_Write, Greater_No_Write, LEqual_No_Write, GEqual_No_Write, Equal_No_Write, NotEqual_No_Write, Always_No_Write };
	//RasterizerState
	enum class RS_State { Cull_Back, Cull_Front, Cull_None, Standard, Wire };
	//BlendState
	enum class BS_State { Off, Alpha, Alpha_Test, Transparent, Add, Subtract, Replace, Multiply };

	class DxSystem
	{
	private:
		static int screen_width;
		static int screen_height;
		static Microsoft::WRL::ComPtr<IDXGISwapChain>			swap_chain;
		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	render_target_view;
		static Microsoft::WRL::ComPtr<ID3D11Texture2D>			depth_stencil_texture;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depth_stencil_view;
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depth_stencil_state[16];

		static Microsoft::WRL::ComPtr<IDXGIDebug>               DXGI_debug;

		static HRESULT Create_Device();
		static bool Create_Depth_Stencil();
		static bool Initialize_Render_Target();

		static const int RASTERIZE_TYPE = 5;
		static Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state[RASTERIZE_TYPE];
		static bool Create_Rasterizer_State();

		static const int BLEND_TYPE = 8;
		static Microsoft::WRL::ComPtr<ID3D11BlendState>	blend_state[BLEND_TYPE];
		static bool Create_Blend_State();

	public:
		static Microsoft::WRL::ComPtr<ID3D11Device>			device;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext>	device_context;
		static HWND hwnd;
		static DXGI_SAMPLE_DESC MSAA;

		static bool Initialize(HWND hWnd, int width, int height);
		static void Release();
		static void Clear(DWORD color = 0x0000FFFF);
		static void Flip(int n = 0);

		static int Get_Screen_Width() { return screen_width; }
		static int Get_Screen_Height() { return screen_height; }
		static ID3D11DepthStencilView* Get_DepthStencilView() { return depth_stencil_view.Get(); }
		static ID3D11RenderTargetView* Get_RenderTargetView() { return render_target_view.Get(); }
		static ID3D11ShaderResourceView* Get_ShaderResourceView() { return shader_resource_view.Get(); }
		static ID3D11DepthStencilState* Get_DephtStencil_State(DS_State state) { return depth_stencil_state[static_cast<int>(state)].Get(); }
		static ID3D11RasterizerState* Get_Rasterizer_State(RS_State state) { return rasterizer_state[static_cast<int>(state)].Get(); }
		static ID3D11BlendState* Get_Blend_State(BS_State state) { return blend_state[static_cast<int>(state)].Get(); }
		static void Set_ViewPort(int width, int height, int Num = 1);
		static void Set_Default_View();
	};
}