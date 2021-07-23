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

		void Play();  //再生(一時停止中の場合は再開),(同一AudioSourceでの同時再生不可)
		void Pause(); //一時停止
		void Stop();  //停止

		void Play_OneShot(float volume = 1.0f, float pitch = 0.0f); //再生(同時再生可能),(再生中制御不可)
		//static void PlayClipAtPoint(const char* filepath, const char* filename, Vector3 position = { 0,0,0 }, float volume = 1.0f, float pitch = 0.0f); //その場でオブジェクトを生成し再生(同時再生可能),(再生中制御不可)

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