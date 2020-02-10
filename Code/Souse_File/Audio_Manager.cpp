#include "Audio_Manager.h"

unique_ptr <AudioEngine> Audio_Manager::m_audEngine;

unique_ptr <SoundEffect> Audio_Manager::BGM_play_eff;
unique_ptr <SoundEffect> Audio_Manager::BGM_title_eff;
unique_ptr <SoundEffectInstance> Audio_Manager::BGM_play;
unique_ptr <SoundEffectInstance> Audio_Manager::BGM_title;

unique_ptr <SoundEffect> Audio_Manager::title;
unique_ptr <SoundEffect> Audio_Manager::attack;
unique_ptr <SoundEffect> Audio_Manager::hit;
unique_ptr <SoundEffect> Audio_Manager::death;

void Audio_Manager::Initialize()
{
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	m_audEngine = make_unique<AudioEngine>(eflags);

	//title = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\�^�C�g����.wav");

	//BGM_play_eff = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\�v���C��BGM.wav");
	//BGM_play = BGM_play_eff->CreateInstance();

	title = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\�^�C�g����.wav");
	hit = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\�G�U��.wav");
	attack = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\�U��.wav");
	death = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\���S.wav");

	BGM_play_eff = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\�v���C��BGM.wav");
	BGM_play = BGM_play_eff->CreateInstance();
	BGM_title_eff = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\�^�C�g��BGM.wav");
	BGM_title = BGM_title_eff->CreateInstance();
}