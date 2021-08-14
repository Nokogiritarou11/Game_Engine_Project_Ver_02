#pragma once
#include "Shader.h"

namespace BeastEngine
{
	class Texture : public BeastEngine::Object
	{
	public:
		static std::shared_ptr<BeastEngine::Texture> Load(const std::string& texture_path); //�e�N�X�`�����t�@�C�����w�肵�ǂݍ���
		void Set(UINT Slot, BeastEngine::Shader::Shader_Type type); //�e�N�X�`�����V�F�[�_�[�ɃX�e�[�W����
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Get_Resource() const { return shader_resource_view; }
		UINT Get_Width() const { return texture2d_desc.Width; }	//�e�N�X�`���̉�����Ԃ�
		UINT Get_Height() const { return texture2d_desc.Height; } //�e�N�X�`���̏c����Ԃ�
		std::string Get_Path() const { return path; }

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>       sampler = nullptr;
		// �e�N�X�`�����
		D3D11_TEXTURE2D_DESC texture2d_desc = {};
		std::string path;
	};
}