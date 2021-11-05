#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

namespace BeastEngine
{
	//�����_�����O�p�e�N�X�`��
	class Render_Texture
	{
	public:
		Render_Texture(const int& x, const int& y, const bool& msaa, const DXGI_FORMAT& format);
		~Render_Texture();

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Get_Texture();                          //�`�挋�ʂ��擾����
		void Update_Copy_Texture() const;                                                         //�e�N�X�`����ʃo�b�t�@�ɃR�s�[����
		Effekseer::Backend::TextureRef& Get_Back_Ground_Texture() { return back_ground_texture; } //�R�s�[�����`�挋�ʂ��擾����
		void Set_Screen_Size(const int& x, const int& y);                                         //�e�N�X�`���̃T�C�Y��ݒ肷��
		void Set_Render_Target();                                                                 //�e�N�X�`���������_�[�^�[�Q�b�g�Ƃ��ăZ�b�g����
		void Clear() const;                                                                       //�e�N�X�`���̃N���A

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

		bool Create_Depth_Stencil();      //�f�v�X�X�e���V���e�N�X�`�����쐬����
		bool Create_Render_Target_View(); //�����_�[�^�[�Q�b�g���쐬����

		DXGI_FORMAT color_format;
		bool use_msaa; //MSAA���g�p���邩
	};
}