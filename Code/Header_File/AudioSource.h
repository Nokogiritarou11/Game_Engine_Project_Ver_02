#pragma once
#include "Audio.h"
#include "Behaviour.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

namespace BeastEngine
{
	//�I�[�f�B�I�̍Đ���S������R���|�[�l���g�N���X
	class AudioSource final : public Behaviour
	{
	public:
		~AudioSource() override;

		void Set_Clip(const char* filepath, const char* filename);

		void Play() const;  //�Đ�(�ꎞ��~���̏ꍇ�͍ĊJ),(����AudioSource�ł̓����Đ��s��)
		void Pause() const; //�ꎞ��~
		void Stop() const;  //��~

		void Play_OneShot(float volume = 1.0f, float pitch = 0.0f) const; //�Đ�(�����Đ��\),(�Đ�������s��)

		[[nodiscard]] bool Is_Playing() const; //�Đ�����

		void Set_Volume(float volume) const;                      //���ʂ�ݒ肷��
		[[nodiscard]] float Get_Volume() const { return volume; } //���ʂ��擾����

		void Set_Pitch(float pitch) const;                        //�Đ����̃s�b�`��ݒ肷��
		[[nodiscard]] float Get_Pitch() const { return pitch; }   //�Đ����̃s�b�`���擾����

		bool play_on_awake = true; //�A�N�e�B�u�����Ɏ����Đ����邩
		bool loop = false;         //���[�v�Đ����邩

	private:
		std::unique_ptr<DirectX::SoundEffectInstance> effect_instance{};
		std::string file_name{};
		std::string file_path{};

		float volume = 1.0f;
		float pitch = 0.0f;

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //������
		void Set_Active(bool value) override;                             //�A�N�e�B�u��Ԃ�؂�ւ���
		bool Draw_ImGui() override;                                       //ImGui�`��
		bool Can_Multiple() override { return true; };                    //���R���|�[�l���g�𕡐��A�^�b�`�\��

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Behaviour>(this), file_name, file_path, play_on_awake, loop, volume, pitch);
		}
	};
}

REGISTER_COMPONENT(AudioSource)
CEREAL_REGISTER_TYPE(BeastEngine::AudioSource)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::AudioSource)
CEREAL_CLASS_VERSION(BeastEngine::AudioSource, 1)