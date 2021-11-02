#pragma once
#include "Audio.h"

namespace BeastEngine
{
	//オーディオ管理マネージャー
	class Audio_Manager
	{
	public:
		Audio_Manager();

		void Update() const;  //更新
		void Suspend() const; //一時停止する
		void Resume() const;  //再開する
		void Reset() const;   //リセットする

		std::unique_ptr<DirectX::SoundEffectInstance> Load_SoundEffect(std::string filename); //ファイルパスからオーディオクリップを読み込む
		void Play_OneShot(std::string filename, float volume = 1.0f, float pitch = 0.0f); //重複可能な音源を再生する

	private:
		std::unique_ptr<DirectX::AudioEngine> engine;
		std::unordered_map<std::wstring, std::unique_ptr<DirectX::SoundEffect>> effect_map;
	};
}