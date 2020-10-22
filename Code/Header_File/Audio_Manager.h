#pragma once
#include "Audio.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <vector>
#include <wrl.h>
#include <stdio.h>
using namespace std;

class Audio_Manager
{
public:
	Audio_Manager();
	unique_ptr <AudioEngine> m_audEngine;

	static unique_ptr <SoundEffect> BGM_play_eff;
	static unique_ptr <SoundEffect> BGM_title_eff;

	static unique_ptr <SoundEffectInstance> BGM_play;
	static unique_ptr <SoundEffectInstance> BGM_title;

	static unique_ptr <SoundEffect> Start;
	static unique_ptr <SoundEffect> Get;
	static unique_ptr <SoundEffect> Damage;
	static unique_ptr <SoundEffect> Boost;
	static unique_ptr <SoundEffect> Clash;

	float volume = 0;

private:
};