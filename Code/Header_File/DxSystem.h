#pragma once
#include <InitGUID.h>
#include <dxgidebug.h>
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include <Windows.h>
#include <string.h>
#include <DirectXMath.h>
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
	static ComPtr<ID3D11DepthStencilState>	DepthStencilState[3];

	static ComPtr<IDXGIDebug>               DXGIDebug;

	static HRESULT CreateDevice();
	static bool CreateDepthStencil();
	static bool InitializeRenderTarget();

	static const int RASTERIZE_TYPE = 6;
	static ComPtr<ID3D11RasterizerState> RasterizerState[RASTERIZE_TYPE];
	static bool CreateRasterizerState();

	static const int BLEND_TYPE = 11;
	static ComPtr<ID3D11BlendState>		BlendState[BLEND_TYPE];
	static bool CreateBlendState();

public:

	//DepthStencilState
	enum class DS_State { DS_FALSE, DS_TRUE, DS_SKY };
	//RasterizerState
	enum class RS_State { RS_STANDARD, RS_CULL_BACK, RS_WIRE, RS_CULL_FRONT, RS_CULL_NONE, RS_CULL_SKY };
	//BlendState
	enum class BS_State { BS_NONE, BS_ALPHA, BS_ALPHA_TEST, BS_TRANSPARENT, BS_ADD, BS_SUBTRACT, BS_REPLACE, BS_MULTIPLY, BS_LIGHTEN, BS_DARKEN, BS_SCREEN };

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
