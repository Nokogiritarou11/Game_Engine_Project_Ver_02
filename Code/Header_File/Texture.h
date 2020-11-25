#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <wrl.h>
#include <string>
#include <clocale>
#include <tchar.h>
using Microsoft::WRL::ComPtr;

class Texture
{
protected:
	ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	ComPtr<ID3D11SamplerState>       sampler            = nullptr;
	ComPtr<ID3D11RenderTargetView>   RenderTargetView   = nullptr;

	// �e�N�X�`�����
	D3D11_TEXTURE2D_DESC texture2d_desc;


public:
	Texture();
	virtual ~Texture();

	bool Load(std::string filename,int sampler_state = 0);							//�e�N�X�`�����t�@�C�����w�肵�ǂݍ���(���s������False��Ԃ�)
	void Set(UINT Slot = 0, BOOL flg = TRUE);										//�e�N�X�`�����V�F�[�_�[�ɃX�e�[�W����
	UINT GetWidth() { return texture2d_desc.Width; }								//�e�N�X�`���̉�����Ԃ�
	UINT GetHeight() { return texture2d_desc.Height; }								//�e�N�X�`���̏c����Ԃ�
	ID3D11RenderTargetView* GetRenderTarget() { return RenderTargetView.Get(); }	//�����_�[�^�[�Q�b�g��Ԃ�

	bool Texture_Have = false;														//�e�N�X�`���f�[�^��ێ����Ă��邩

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
