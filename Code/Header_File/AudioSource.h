#pragma once
#include "Audio.h"
#include "Behaviour.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include <stdio.h>

class AudioSource : public Behaviour
{
public:
	~AudioSource();

	void Set_Clip(const char* filepath, const char* filename);

	void Play();  //再生(一時停止中の場合は再開),(同一AudioSourceでの同時再生不可)
	void Pause(); //一時停止
	void Stop();  //停止

	void PlayOneShot(float volume = 1.0f, float pitch = 0.0f); //再生(同時再生可能),(再生中制御不可)
	//static void PlayClipAtPoint(const char* filepath, const char* filename, Vector3 position = { 0,0,0 }, float volume = 1.0f, float pitch = 0.0f); //その場でオブジェクトを生成し再生(同時再生可能),(再生中制御不可)

	bool IsPlaying();
	void Set_Volume(float volume);
	float Get_Volume() { return Volume; };
	void Set_Pitch(float pitch);
	float Get_Pitch() { return Pitch; };

	bool Play_On_Awake = true;
	bool Loop = false;

private:

	std::unique_ptr<DirectX::SoundEffectInstance> Effect_Instance;
	std::string file_name;
	std::string file_path;

	float Volume = 1.0f;
	float Pitch = 0.0f;

	void Initialize(std::shared_ptr<GameObject> obj) override;
	void SetActive(bool value) override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Behaviour>(this), file_name, file_path, Play_On_Awake, Loop, Volume, Pitch);
	}
};
CEREAL_REGISTER_TYPE(AudioSource)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Behaviour, AudioSource)