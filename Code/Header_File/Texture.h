#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <unordered_map>
#include "Object.h"

namespace BeastEngine
{
	class Texture : public BeastEngine::Object
	{
	public:
		static std::shared_ptr<BeastEngine::Texture> Load(std::string filename);		//�e�N�X�`�����t�@�C�����w�肵�ǂݍ���(���s������False��Ԃ�)
		void Set(UINT Slot = 0, BOOL flg = TRUE);										//�e�N�X�`�����V�F�[�_�[�ɃX�e�[�W����
		UINT Get_Width() { return texture2d_desc.Width; }								//�e�N�X�`���̉�����Ԃ�
		UINT Get_Height() { return texture2d_desc.Height; }								//�e�N�X�`���̏c����Ԃ�
		ID3D11RenderTargetView* Get_Render_Target() { return render_target_view.Get(); }//�����_�[�^�[�Q�b�g��Ԃ�												//�e�N�X�`���f�[�^��ێ����Ă��邩

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>       sampler = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   render_target_view = nullptr;

		// �e�N�X�`�����
		D3D11_TEXTURE2D_DESC texture2d_desc;
	};
}