#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <wrl.h>
#include <string>
#include <clocale>
#include <tchar.h>
using Microsoft::WRL::ComPtr;

namespace BeastEngine
{
	class Texture
	{
	protected:
		ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
		ComPtr<ID3D11SamplerState>       sampler = nullptr;
		ComPtr<ID3D11RenderTargetView>   render_target_view = nullptr;

		// �e�N�X�`�����
		D3D11_TEXTURE2D_DESC texture2d_desc;


	public:
		Texture();
		virtual ~Texture();

		bool load(std::string filename, int sampler_state = 0);							//�e�N�X�`�����t�@�C�����w�肵�ǂݍ���(���s������False��Ԃ�)
		void Set(UINT Slot = 0, BOOL flg = TRUE);										//�e�N�X�`�����V�F�[�_�[�ɃX�e�[�W����
		UINT Get_Width() { return texture2d_desc.Width; }								//�e�N�X�`���̉�����Ԃ�
		UINT Get_Height() { return texture2d_desc.Height; }								//�e�N�X�`���̏c����Ԃ�
		ID3D11RenderTargetView* Get_Render_Target() { return render_target_view.Get(); }	//�����_�[�^�[�Q�b�g��Ԃ�

		bool has_texture = false;														//�e�N�X�`���f�[�^��ێ����Ă��邩

		enum class Texture_Type
		{
			Main,
			Specular,
			Normal,
			Height,
			Emission,
			//�ǉ�������Material�̃e�N�X�`���z��Ə��z��̃T�C�Y,Active_Texture()�����������邱��
		};
	};
}