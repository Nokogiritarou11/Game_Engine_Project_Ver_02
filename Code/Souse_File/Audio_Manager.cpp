#include "Audio_Manager.h"
#include <clocale>
#include <tchar.h>
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

Audio_Manager::Audio_Manager()
{
	CoInitializeEx(nullptr,COINIT_MULTITHREADED);
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_EnvironmentalReverb | AudioEngine_ReverbUseFilters | AudioEngine_UseMasteringLimiter;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	Engine = make_unique<AudioEngine>(eflags);
}

void Audio_Manager::Update()
{
	if (!Engine->Update())
	{
		if (Engine->IsCriticalError())
		{
			Engine->Reset();
		}
	}
}

void Audio_Manager::Resume()
{
	Engine->Resume();
}

void Audio_Manager::Suspend()
{
	Engine->Suspend();
}

void Audio_Manager::Reset()
{
	Engine->TrimVoicePool();
	Engine->Reset();
}

unique_ptr<DirectX::SoundEffectInstance> Audio_Manager::Load_SoundEffect(string filename)
{
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	auto it = Effect_Map.find(FileName);
	if (it != Effect_Map.end())
	{
		return move(it->second->CreateInstance());
	}
	else
	{
		Effect_Map.insert(make_pair(FileName, make_unique<SoundEffect>(Engine.get(), FileName)));
		return move(Effect_Map.find(FileName)->second->CreateInstance());
	}

	return nullptr;
}

void Audio_Manager::PlayOneShot(std::string filename, float volume, float pitch)
{
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	auto it = Effect_Map.find(FileName);
	if (it != Effect_Map.end())
	{
		it->second->Play(volume, pitch, 0.0f);
	}
	else
	{
		Effect_Map.insert(make_pair(FileName, make_unique<SoundEffect>(Engine.get(), FileName)));
		Effect_Map.find(FileName)->second->Play(volume, pitch, 0.0f);
	}
}