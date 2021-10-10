#pragma once
#include "Audio.h"

namespace BeastEngine
{
	class Audio_Manager
	{
	public:
		Audio_Manager();

		void Update() const;
		void Suspend() const;
		void Resume() const;
		void Reset() const;

		std::unique_ptr<DirectX::SoundEffectInstance> Load_SoundEffect(std::string filename);
		void Play_OneShot(std::string filename, float volume = 1.0f, float pitch = 0.0f);

	private:
		std::unique_ptr<DirectX::AudioEngine> engine;
		std::unordered_map<std::wstring, std::unique_ptr<DirectX::SoundEffect>> effect_map;
	};
}