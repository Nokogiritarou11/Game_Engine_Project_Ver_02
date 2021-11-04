#pragma once
#include <dxgidebug.h>
#include <d3d11.h>
#include <wrl.h>
#include <Windows.h>
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

	//DirectX���b�p�[�N���X
	class DxSystem
	{
	public:
		static Microsoft::WRL::ComPtr<ID3D11Device>			device;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext>	device_context;
		static HWND hwnd;
		static DXGI_SAMPLE_DESC MSAA; //MSAA�g�p���̃p�����[�^

		static bool Initialize(HWND hWnd, int width, int height); //������
		static void Release(); //��n��
		static void Clear(DWORD color = 0x0000FFFF); //�����_�[�^�[�Q�b�g�̃N���A
		static void Flip(int n = 0); //�t���b�v����

		static int Get_Screen_Width() { return screen_width; }   //�E�B���h�E�̉������擾
		static int Get_Screen_Height() { return screen_height; } //�E�B���h�E�̏c�����擾
		static ID3D11DepthStencilView* Get_DepthStencilView() { return depth_stencil_view.Get(); }
		static ID3D11RenderTargetView* Get_RenderTargetView() { return render_target_view.Get(); }
		static ID3D11ShaderResourceView* Get_ShaderResourceView() { return shader_resource_view.Get(); }
		static ID3D11DepthStencilState* Get_DepthStencil_State(DS_State state) { return depth_stencil_state[static_cast<int>(state)].Get(); }
		static ID3D11RasterizerState* Get_Rasterizer_State(RS_State state) { return rasterizer_state[static_cast<int>(state)].Get(); }
		static ID3D11BlendState* Get_Blend_State(BS_State state) { return blend_state[static_cast<int>(state)].Get(); }
		static void Set_ViewPort(int width, int height, int Num = 1); //�r���[�|�[�g���Z�b�g����
		static void Set_Default_View(); //���C���E�B���h�E�̃����_�[�^�[�Q�b�g���Z�b�g����

	private:
		static int screen_width;
		static int screen_height;
		static Microsoft::WRL::ComPtr<IDXGISwapChain>			swap_chain;
		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	render_target_view;
		static Microsoft::WRL::ComPtr<ID3D11Texture2D>			depth_stencil_texture;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depth_stencil_view;
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depth_stencil_state[16];

		static Microsoft::WRL::ComPtr<IDXGIDebug>               dxgi_debug;

		static HRESULT Create_Device();         //�f�o�C�X�̍쐬
		static bool Create_Depth_Stencil();     //�f�v�X�X�e���V���e�N�X�`���̍쐬
		static bool Initialize_Render_Target(); //�����_�[�^�[�Q�b�g�̏�����

		static constexpr int rasterizer_type = 5; //�p�ӂ��ꂽ���X�^���C�U�[�X�e�[�g�̐�
		static Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state[rasterizer_type];
		static bool Create_Rasterizer_State(); //���X�^���C�U�[�X�e�[�g�̍쐬

		static constexpr int blend_type = 8; //�p�ӂ��ꂽ�u�����h�X�e�[�g�̐�
		static Microsoft::WRL::ComPtr<ID3D11BlendState>	blend_state[blend_type];
		static bool Create_Blend_State(); //�u�����h�X�e�[�g�̍쐬

	};
}