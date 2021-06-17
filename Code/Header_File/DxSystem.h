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
using Microsoft::WRL::ComPtr;

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
		static int ScreenWidth;
		static int ScreenHeight;
		static ComPtr<IDXGISwapChain>			SwapChain;
		static ComPtr<ID3D11RenderTargetView>	RenderTargetView;
		static ComPtr<ID3D11Texture2D>			DepthStencilTexture;
		static ComPtr<ID3D11DepthStencilView>	DepthStencilView;
		static ComPtr<ID3D11ShaderResourceView>	ShaderResourceView;
		static ComPtr<ID3D11DepthStencilState>	DepthStencilState[16];

		static ComPtr<IDXGIDebug>               DXGIDebug;

		static HRESULT CreateDevice();
		static bool CreateDepthStencil();
		static bool InitializeRenderTarget();

		static const int RASTERIZE_TYPE = 5;
		static ComPtr<ID3D11RasterizerState> RasterizerState[RASTERIZE_TYPE];
		static bool CreateRasterizerState();

		static const int BLEND_TYPE = 8;
		static ComPtr<ID3D11BlendState>		BlendState[BLEND_TYPE];
		static bool CreateBlendState();

	public:
		static ComPtr<ID3D11Device>			Device;
		static ComPtr<ID3D11DeviceContext>	DeviceContext;
		static HWND hwnd;
		static DXGI_SAMPLE_DESC MSAA;

		static DirectX::XMFLOAT4 Light_Direction;
		static float elapsed_time;

		static bool Initialize(HWND hWnd, int width, int height);
		static void Release();
		static void Clear(DWORD color = 0x0000FFFF);
		static void Flip(int n = 0);

		static int GetScreenWidth() { return ScreenWidth; }
		static int GetScreenHeight() { return ScreenHeight; }
		static ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }
		static ID3D11RenderTargetView* GetRenderTargetView() { return RenderTargetView.Get(); }
		static ID3D11DepthStencilState* GetDephtStencilState(DS_State state) { return DepthStencilState[static_cast<int>(state)].Get(); }
		static ID3D11RasterizerState* GetRasterizerState(RS_State state) { return RasterizerState[static_cast<int>(state)].Get(); }
		static ID3D11BlendState* GetBlendState(BS_State state) { return BlendState[static_cast<int>(state)].Get(); }
		static void SetViewPort(int width, int height, int Num = 1);
		static void SetDefaultView();
	};
}