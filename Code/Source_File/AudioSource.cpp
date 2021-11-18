#include "AudioSource.h"
#include "GameObject.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Audio_Manager.h"
#include "Include_ImGui.h"
#include "System_Function.h"
#include "Mathf.h"
#include "Debug.h"
#include "Asset_Manager.h"
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

AudioSource::~AudioSource()
{
	if (effect_instance)
	{
		if (effect_instance->GetState() == PLAYING)
		{
			effect_instance->Stop();
		}
		effect_instance.reset();
	}
}

void AudioSource::Initialize(const shared_ptr<GameObject>& obj)
{
	Engine::asset_manager->Registration_Asset(shared_from_this());
	gameobject = obj;
	transform = obj->transform;

	if (file_path != "")
	{
		effect_instance = Engine::audio_manager->Load_SoundEffect(file_path + file_name);
		Set_Volume(volume);
		Set_Pitch(pitch);
	}
	Set_Active(Get_Enabled());
}

void AudioSource::Set_Active(const bool value)
{
	if (value)
	{
		if (play_on_awake && gameobject->Get_Active_In_Hierarchy() && Get_Enabled() && Engine::scene_manager->run)
		{
			Play();
		}
	}
	else
	{
		Stop();
	}
}

void AudioSource::Set_Clip(const char* filepath, const char* filename)
{
	if (effect_instance)
	{
		if (effect_instance->GetState() == PLAYING)
		{
			effect_instance->Stop();
		}
	}
	file_name = filename;
	file_path = filepath;
	effect_instance = Engine::audio_manager->Load_SoundEffect(file_path + file_name);
	Set_Volume(volume);
	Set_Pitch(pitch);
}

void AudioSource::Play() const
{
	if (effect_instance)
	{
		const auto state = effect_instance->GetState();
		Set_Volume(volume);
		Set_Pitch(pitch);
		switch (state)
		{
			case PAUSED:
				effect_instance->Resume();
				break;
			case PLAYING:
				effect_instance->Stop();
				effect_instance->Play(loop);
				break;
			case STOPPED:
				effect_instance->Play(loop);
				break;
		}
	}
}

void AudioSource::Pause() const
{
	if (effect_instance)
	{
		effect_instance->Pause();
	}
}

void AudioSource::Stop() const
{
	if (effect_instance)
	{
		effect_instance->Stop();
	}
}

void AudioSource::Play_OneShot(float volume, float pitch) const
{
	if (file_path != "")
	{
		Engine::audio_manager->Play_OneShot(file_path + file_name, volume, pitch);
	}
}

/*
void AudioSource::PlayClipAtPoint(const char* filepath, const char* filename, Vector3 position, float volume, float pitch)
{
	string path = filepath;
	path += filename;
	Engine::audio_manager->PlayOneShot(path, volume, pitch);
}
*/

bool AudioSource::Is_Playing() const
{
	if (effect_instance)
	{
		if (const auto state = effect_instance->GetState(); state == PLAYING)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void AudioSource::Set_Volume(float volume) const
{
	volume = max(0, volume);
	if (effect_instance)
	{
		effect_instance->SetVolume(volume);
	}
}
void AudioSource::Set_Pitch(float pitch) const
{
	pitch = Mathf::Clamp(pitch, -1.0f, 1.0f);
	if (effect_instance)
	{
		effect_instance->SetPitch(pitch);
	}
}

bool AudioSource::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("AudioSource", open)) return false;

	if (open)
	{
		ImGui::Text(u8"現在のファイル::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());
		if (ImGui::Button(u8"Wavファイルを選択"))
		{
			if (const string path = System_Function::Get_Open_File_Name("wav","\\Assets\\Audio"); path != "")
			{
				const int path_i = path.find_last_of("\\") + 1;
				const int ext_i = path.find_last_of(".");
				const string path_name = path.substr(0, path_i); //ファイルまでのディレクトリ
				const string ext_name = path.substr(ext_i, path.size() - ext_i); //拡張子
				const string file_name = path.substr(path_i, ext_i - path_i); //ファイル名
				const string name = file_name + ext_name;
				if (ext_name == ".wav")
				{
					Set_Clip(path_name.c_str(), name.c_str());
				}
				else
				{
					Debug::Log("ファイル形式が対応していません");
				}
			}
		}

		ImGui::Dummy({0,3});
		ImGui::Text(u8"音量　");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		const bool Input_volume = ImGui::DragFloat(u8"##音量", &volume, 0.01f, 0.0f, 5.0f, "%.2f");
		ImGui::SameLine();
		const bool slide_volume = ImGui::SliderFloat(u8"##音量", &volume, 0.0f, 5.0f, "%.2f");
		if (Input_volume || slide_volume)
		{
			if (effect_instance)
			{
				if (effect_instance->GetState() == SoundState::PLAYING)
				{
					Set_Volume(volume);
				}
			}
		}

		ImGui::Text(u8"ピッチ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		const bool Input_pitch = ImGui::DragFloat(u8"##ピッチ", &pitch, 0.01f, -1.0f, 1.0f, "%.2f");
		ImGui::SameLine();
		const bool slide_pitch = ImGui::SliderFloat(u8"##ピッチ", &pitch, -1.0f, 1.0f, "%.2f");
		if (Input_pitch || slide_pitch)
		{
			if (effect_instance)
			{
				if (effect_instance->GetState() == SoundState::PLAYING)
				{
					Set_Pitch(pitch);
				}
			}
		}

		ImGui::Dummy({0,3});
		ImGui::Checkbox(u8"アクティブ時の自動再生", &play_on_awake);
		ImGui::Dummy({ 0,3 });
		ImGui::Checkbox(u8"ループ再生", &loop);

		ImGui::Dummy({ 0,3 });
		if (effect_instance)
		{
			if (ImGui::Button(ICON_FA_PLAY, ImVec2(30, 0)))
			{
				Play();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PAUSE, ImVec2(30, 0)))
			{
				Pause();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_STOP, ImVec2(30, 0)))
			{
				Stop();
			}
		}
	}
	return true;
}