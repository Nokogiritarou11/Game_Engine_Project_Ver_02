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
	engine = make_unique<AudioEngine>(eflags);
}

void Audio_Manager::Update()
{
	if (!engine->Update())
	{
		if (engine->IsCriticalError())
		{
			engine->Reset();
		}
	}
}

void Audio_Manager::Resume()
{
	engine->Resume();
}

void Audio_Manager::Suspend()
{
	engine->Suspend();
}

void Audio_Manager::Reset()
{
	engine->TrimVoicePool();
	engine->Reset();
}

unique_ptr<DirectX::SoundEffectInstance> Audio_Manager::Load_SoundEffect(string filename)
{
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	auto it = effect_map.find(FileName);
	if (it != effect_map.end())
	{
		return move(it->second->CreateInstance());
	}
	else
	{
		effect_map.insert(make_pair(FileName, make_unique<SoundEffect>(engine.get(), FileName)));
		return move(effect_map.find(FileName)->second->CreateInstance());
	}

	return nullptr;
}

void Audio_Manager::Play_OneShot(std::string filename, float volume, float pitch)
{
	setlocale(LC_ALL, "japanese");
	wchar_t FileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, FileName, MAX_PATH, filename.c_str(), _TRUNCATE);

	auto it = effect_map.find(FileName);
	if (it != effect_map.end())
	{
		it->second->Play(volume, pitch, 0.0f);
	}
	else
	{
		effect_map.insert(make_pair(FileName, make_unique<SoundEffect>(engine.get(), FileName)));
		effect_map.find(FileName)->second->Play(volume, pitch, 0.0f);
	}
}