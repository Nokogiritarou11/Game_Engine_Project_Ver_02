#include "Audio_Manager.h"

/*
unique_ptr <SoundEffect> Audio_Manager::BGM_play_eff;
unique_ptr <SoundEffect> Audio_Manager::BGM_title_eff;

unique_ptr <SoundEffectInstance> Audio_Manager::BGM_play;
unique_ptr <SoundEffectInstance> Audio_Manager::BGM_title;

unique_ptr <SoundEffect> Audio_Manager::title;
unique_ptr <SoundEffect> Audio_Manager::attack;
unique_ptr <SoundEffect> Audio_Manager::hit;
unique_ptr <SoundEffect> Audio_Manager::death;
*/

Audio_Manager::Audio_Manager()
{
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags |= AudioEngine_Debug;
#endif
	m_audEngine = make_unique<AudioEngine>(eflags);

	//title = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\タイトル音.wav");

	//BGM_play_eff = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\プレイ中BGM.wav");
	//BGM_play = BGM_play_eff->CreateInstance();

	/*
	title = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\タイトル音.wav");
	hit = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\敵攻撃.wav");
	attack = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\攻撃.wav");
	death = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\死亡.wav");

	BGM_play_eff = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\プレイ中BGM.wav");
	BGM_play = BGM_play_eff->CreateInstance();
	BGM_title_eff = make_unique<SoundEffect>(m_audEngine.get(), L"Audio\\タイトルBGM.wav");
	BGM_title = BGM_title_eff->CreateInstance();
	*/

}