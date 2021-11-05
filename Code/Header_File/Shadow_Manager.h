#pragma once
#include <memory>
#include "DxSystem.h"
#include <wrl.h>

namespace BeastEngine
{
	class Material;
	class Shader;
	class Gaussian_Filter;
	class Render_Texture;

	//�e�����_�����O�Ǘ��N���X
	class Shadow_Manager
	{
	public:
		Shadow_Manager();

		void Set_PS_Resource();                       //�s�N�Z���V�F�[�_�[�Ƀ��\�[�X���Z�b�g����
		void Set_Shadow_Map_Texture() const;          //�V���h�E�}�b�v�e�N�X�`�����X�e�[�W����
		void Set_Default_Shadow_Alpha() const;        //�V���h�E�}�b�v�̃A���t�@�N���b�v�p�e�N�X�`�����X�e�[�W����
		void Set_Shadow_Map_Texture_Size(u_int size); //�V���h�E�}�b�v�̃e�N�X�`���T�C�Y��ݒ肷��
		void Filtering_Gaussian() const;              //�V���h�E�}�b�v���ڂ���

		[[nodiscard]] u_int Get_Shadow_Map_Texture_Size() const { return shadow_map_texture_size; }
		float shadow_bias = 0.01f;
		float shadow_distance = 15;

	private:
		std::unique_ptr<Render_Texture> render_texture;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_shadow;
		std::shared_ptr<Material> material_shadow;
		std::unique_ptr<Gaussian_Filter> gaussian_filter;

		u_int shadow_map_texture_size = 2048;
	};
}