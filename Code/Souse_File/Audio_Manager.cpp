#include "Audio_Manager.h"

unique_ptr <SoundEffect> Audio_Manager::BGM_play_eff;
unique_ptr <SoundEffect> Audio_Manager::BGM_title_eff;

unique_ptr <SoundEffectInstance> Audio_Manager::BGM_play;
unique_ptr <SoundEffectInstance> Audio_Manager::BGM_title;

unique_ptr <SoundEffect> Audio_Manager::Start;
unique_ptr <SoundEffect> Audio_Manager::Get;
unique_ptr <SoundEffect> Audio_Manager::Damage;
unique_ptr <SoundEffect> Audio_Manager::Boost;
unique_ptr <SoundEffect> Audio_Manager::Clash;

Audio_Manager::Audio_Manager()
{
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags |= AudioEngine_Debug;
#endif
	m_audEngine = make_unique<AudioEngine>(eflags);

	Start = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\SE\\start.wav");
	Get = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\SE\\アイテム入手.wav");
	Damage = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\SE\\あたった時の音.wav");
	Boost = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\SE\\ブースト.wav");
	Clash = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\SE\\崩れる音.wav");

	BGM_play_eff = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\BGM\\BGM.wav");
	BGM_play = BGM_play_eff->CreateInstance();
	BGM_title_eff = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\BGM\\title2.wav");
	BGM_title = BGM_title_eff->CreateInstance();

}