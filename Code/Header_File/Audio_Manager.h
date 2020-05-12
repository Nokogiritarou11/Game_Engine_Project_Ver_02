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
	static unique_ptr <AudioEngine> m_audEngine;

	static unique_ptr <SoundEffect> BGM_play_eff;
	static unique_ptr <SoundEffect> BGM_title_eff;

	static unique_ptr <SoundEffectInstance> BGM_play;
	static unique_ptr <SoundEffectInstance> BGM_title;

	static unique_ptr <SoundEffect> title;
	static unique_ptr <SoundEffect> attack;
	static unique_ptr <SoundEffect> hit;
	static unique_ptr <SoundEffect> death;

	float volume = 0;

	static void Initialize();

private:
};