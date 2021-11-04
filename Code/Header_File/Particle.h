#pragma once
#include "Component.h"
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "DxSystem.h"
#include "Original_Math.h"
#include <memory>


namespace BeastEngine
{
	//Effekseer�̃G�t�F�N�g��ǂݍ��ݕ\������p�[�e�B�N���R���|�[�l���g
	class Particle final : public Component
	{
	public:
		~Particle() override;

		void Set_Particle(const char* filepath, const char* filename);
		[[nodiscard]] bool Get_Is_Playing() const { return is_playing; }

		void Play();  //�Đ�(�ꎞ��~���̏ꍇ�͍ĊJ)
		void Pause() const; //�ꎞ��~
		void Stop();  //��~

		bool play_on_awake = true; //�A�N�e�B�u���Ɏ����ōĐ����J�n���邩
		float play_speed = 1.0f;   //�Đ����x

	private:
		void Initialize(const std::shared_ptr<GameObject>& obj) override; //������
		bool Draw_ImGui() override; //ImGui�`��
		void Set_Active(bool value) override; //�A�N�e�B�u��Ԃ�ݒ肷��
		bool Can_Multiple() override { return false; }; //���R���|�[�l���g�𕡐��A�^�b�`�ł��邩

		bool is_called = false; //���Ƀ}�l�[�W���[�ɓo�^����Ă��邩
		bool is_playing = false; //�Đ�����

		Effekseer::EffectRef effect = nullptr;
		Effekseer::Handle handle = -1;

		std::string file_name;
		std::string file_path;

		friend class Particle_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), play_on_awake, play_speed, file_name, file_path);
		}
	};
}

REGISTER_COMPONENT(Particle)
CEREAL_REGISTER_TYPE(BeastEngine::Particle)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Particle)
CEREAL_CLASS_VERSION(BeastEngine::Particle, 1)