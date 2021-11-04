#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <memory>
#include <vector>

namespace BeastEngine
{
	class Particle;
	class Transform;

	//Effekseer�̘c�ݕ`��p�R�[���o�b�N
	class Distorting_Callback final : public EffekseerRenderer::DistortingCallback
	{
	public:
		bool OnDistorting(EffekseerRenderer::Renderer* renderer) override; //�c�݃G�t�F�N�g���`�悳���ۂɌĂ΂��
	};

	//�p�[�e�B�N���}�l�[�W���[(Effekseer���C�u�����̃��b�p�[)
	class Particle_Manager
	{
	public:
		Particle_Manager();
		~Particle_Manager();

		void Add(std::weak_ptr<Particle> particle); //�p�[�e�B�N���R���|�[�l���g��o�^����
		void Camera_Update(const std::shared_ptr<Transform>& camera_trans, float fov, float near_z, float far_z, float aspect) const; //Effekseer���̃J�������X�V����
		void Update(); //�X�V
		void Render() const; //�`����s
		void Reset(); //�N���A

		EffekseerRendererDX11::RendererRef renderer = nullptr;
		Effekseer::ManagerRef manager = nullptr;
		std::unordered_map<std::string, Effekseer::EffectRef> effect_cache;
		Distorting_Callback* distorting_callback;

	private:
		std::vector<std::weak_ptr<Particle>> particle_list;
		Effekseer::Matrix44 reverse;
	};

}