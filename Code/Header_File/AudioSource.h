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

	void Play();  //�Đ�(�ꎞ��~���̏ꍇ�͍ĊJ),(����AudioSource�ł̓����Đ��s��)
	void Pause(); //�ꎞ��~
	void Stop();  //��~

	void PlayOneShot(float volume = 1.0f, float pitch = 0.0f); //�Đ�(�����Đ��\),(�Đ�������s��)
	//static void PlayClipAtPoint(const char* filepath, const char* filename, Vector3 position = { 0,0,0 }, float volume = 1.0f, float pitch = 0.0f); //���̏�ŃI�u�W�F�N�g�𐶐����Đ�(�����Đ��\),(�Đ�������s��)

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