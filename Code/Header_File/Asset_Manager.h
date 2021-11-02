#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "DxSystem.h"

namespace BeastEngine
{
	class Object;
	class Mesh;
	class Shader;
	class Compute_Shader;
	class Texture;
	class Material;
	class Animation_Clip;

	//Object�p���N���X��A�Z�b�g�̃L���b�V�����Ǘ�����N���X
	class Asset_Manager
	{
	public:
		void Registration_Asset(std::shared_ptr<Object> obj);           //Object��ID�Ɗ֘A�t���ă��X�g�ɓo�^����
		void Erase_Asset(const std::string& id);                        //���X�g����폜����
		std::weak_ptr<Object> Get_Asset_From_Id(const std::string& id); //instance_id����o�^����Ă���Object���擾����

		std::unordered_map<std::string, std::shared_ptr<Mesh>> cache_mesh {};                                 //���b�V���f�[�^�̃L���b�V��
		std::unordered_map<std::string, std::shared_ptr<Shader>> cache_shader {};                             //�V�F�[�_�[�̃L���b�V��
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>> cache_compute_shader {}; //�R���s���[�g�V�F�[�_�[�̃L���b�V��
		std::unordered_map<std::string, std::shared_ptr<Material>> cache_material {};                         //�}�e���A���̃L���b�V��
		std::unordered_map<std::string, std::shared_ptr<Texture>> cache_texture {};                           //�e�N�X�`���f�[�^�̃L���b�V��
		std::unordered_map<std::string, std::shared_ptr<Animation_Clip>> cache_clip {};                       //�A�j���[�V�����N���b�v�̃L���b�V��

		void Exit(); //��n��

	private:
		std::unordered_map<std::string, std::weak_ptr<Object>> cache_asset {};
	};
}