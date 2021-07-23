#pragma once
#include "Audio.h"
#include "Behaviour.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include <stdio.h>

namespace BeastEngine
{
	class AudioSource : public BeastEngine::Behaviour
	{
	public:
		~AudioSource();

		void Set_Clip(const char* filepath, const char* filename);

		void Play();  //�Đ�(�ꎞ��~���̏ꍇ�͍ĊJ),(����AudioSource�ł̓����Đ��s��)
		void Pause(); //�ꎞ��~
		void Stop();  //��~

		void Play_OneShot(float volume = 1.0f, float pitch = 0.0f); //�Đ�(�����Đ��\),(�Đ�������s��)
		//static void PlayClipAtPoint(const char* filepath, const char* filename, Vector3 position = { 0,0,0 }, float volume = 1.0f, float pitch = 0.0f); //���̏�ŃI�u�W�F�N�g�𐶐����Đ�(�����Đ��\),(�Đ�������s��)

		bool Is_Playing();
		void Set_Volume(float volume);
		float Get_Volume() { return volume; };
		void Set_Pitch(float pitch);
		float Get_Pitch() { return pitch; };

		bool play_on_awake = true;
		bool loop = false;

	private:

		std::unique_ptr<DirectX::SoundEffectInstance> effect_instance;
		std::string file_name;
		std::string file_path;

		float volume = 1.0f;
		float pitch = 0.0f;

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		void Set_Active(bool value) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return true; };

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Behaviour>(this), file_name, file_path, play_on_awake, loop, volume, pitch);
		}
	};
}

REGISTER_COMPONENT(AudioSource)
CEREAL_REGISTER_TYPE(BeastEngine::AudioSource)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::AudioSource)
CEREAL_CLASS_VERSION(BeastEngine::AudioSource, 1)