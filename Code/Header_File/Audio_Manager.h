#pragma once
#include "Audio.h"
#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include <stdio.h>

namespace BeastEngine
{
	class Audio_Manager
	{
	public:
		Audio_Manager();

		void Update();
		void Suspend();
		void Resume();
		void Reset();

		std::unique_ptr<DirectX::SoundEffectInstance> Load_SoundEffect(std::string filename);
		void PlayOneShot(std::string filename, float volume = 1.0f, float pitch = 0.0f);

	private:
		std::unique_ptr<DirectX::AudioEngine> Engine;
		std::unordered_map<std::wstring, std::unique_ptr<DirectX::SoundEffect>> Effect_Map;
	};
}