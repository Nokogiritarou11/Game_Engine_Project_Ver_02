#pragma once
#include "Shader.h"

namespace BeastEngine
{
	class Texture : public Object
	{
	public:
		static std::shared_ptr<Texture> Load(const std::string& texture_path); //�e�N�X�`�����t�@�C�����w�肵�ǂݍ���
		void Set(UINT Slot, Shader::Shader_Type type); //�e�N�X�`�����V�F�[�_�[�ɃX�e�[�W����
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Get_Resource() const { return shader_resource_view; }
		[[nodiscard]] UINT Get_Width() const { return texture2d_desc.Width; }	//�e�N�X�`���̉�����Ԃ�
		[[nodiscard]] UINT Get_Height() const { return texture2d_desc.Height; } //�e�N�X�`���̏c����Ԃ�
		[[nodiscard]] std::string Get_Path() const { return path; }

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>       sampler = nullptr;
		// �e�N�X�`�����
		D3D11_TEXTURE2D_DESC texture2d_desc = {};
		std::string path;
	};
}