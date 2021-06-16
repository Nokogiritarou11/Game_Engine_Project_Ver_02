#include "AudioSource.h"
#include "GameObject.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Audio_Manager.h"
#include "Include_ImGui.h"
#include "System_Function.h"
#include "Mathf.h"
#include "Debug.h"
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

AudioSource::~AudioSource()
{
	if (Effect_Instance)
	{
		auto state = Effect_Instance->GetState();
		if (state == SoundState::PLAYING)
		{
			Effect_Instance->Stop();
		}
		Effect_Instance.reset();
	}
}

void AudioSource::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;

	if (file_path != "")
	{
		Effect_Instance = Engine::audio_manager->Load_SoundEffect(file_path + file_name);
		Set_Volume(Volume);
		Set_Pitch(Pitch);
	}
	SetActive(enableSelf());
}

void AudioSource::SetActive(bool value)
{
	if (value)
	{
		if (Play_On_Awake && gameObject->activeInHierarchy() && enableSelf() && Engine::scene_manager->Run)
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
	if (Effect_Instance)
	{
		auto state = Effect_Instance->GetState();
		if (state == SoundState::PLAYING)
		{
			Effect_Instance->Stop();
		}
	}
	file_name = filename;
	file_path = filepath;
	Effect_Instance = Engine::audio_manager->Load_SoundEffect(file_path + file_name);
	Set_Volume(Volume);
	Set_Pitch(Pitch);
}

void AudioSource::Play()
{
	if (Effect_Instance)
	{
		auto state = Effect_Instance->GetState();
		Set_Volume(Volume);
		Set_Pitch(Pitch);
		if (state == SoundState::PAUSED)
		{
			Effect_Instance->Resume();
		}
		else if (state == SoundState::PLAYING)
		{
			Effect_Instance->Stop();
			Effect_Instance->Play(Loop);
		}
		else
		{
			Effect_Instance->Play(Loop);
		}
	}
}

void AudioSource::Pause()
{
	if (Effect_Instance)
	{
		Effect_Instance->Pause();
	}
}

void AudioSource::Stop()
{
	if (Effect_Instance)
	{
		Effect_Instance->Stop();
	}
}

void AudioSource::PlayOneShot(float volume, float pitch)
{
	if (file_path != "")
	{
		Engine::audio_manager->PlayOneShot(file_path + file_name, volume, pitch);
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

bool AudioSource::IsPlaying()
{
	if (Effect_Instance)
	{
		auto state = Effect_Instance->GetState();
		if (state == SoundState::PLAYING)
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

void AudioSource::Set_Volume(float volume)
{
	Volume = max(0, volume);
	if (Effect_Instance)
	{
		Effect_Instance->SetVolume(Volume);
	}
}
void AudioSource::Set_Pitch(float pitch)
{
	Pitch = Mathf::Clamp(pitch, -1.0f, 1.0f);
	if (Effect_Instance)
	{
		Effect_Instance->SetPitch(Pitch);
	}
}

bool AudioSource::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("AudioSource", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = true;
	if (ImGui::BeginPopupContextItem("AudioSource_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<AudioSource>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
	}
	if (!removed)
	{
		return false;
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	static bool enable;
	enable = enableSelf();
	if (ImGui::Checkbox("##enable", &enable))
	{
		SetEnabled(enable);
	}

	if (open)
	{
		ImGui::Text(u8"現在のファイル::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());
		if (ImGui::Button(u8"Wavファイルを選択"))
		{
			string path = System_Function::Get_Open_File_Name();
			if (path != "")
			{
				int path_i = path.find_last_of("\\") + 1;
				int ext_i = path.find_last_of(".");
				string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
				string extname = path.substr(ext_i, path.size() - ext_i); //拡張子
				string filename = path.substr(path_i, ext_i - path_i); //ファイル名
				string name = filename + extname;
				if (extname == ".wav")
				{
					Set_Clip(pathname.c_str(), name.c_str());
				}
				else
				{
					Debug::Log("ファイル形式が対応していません");
				}
			}
		}

		for (int i = 0; i < 5; ++i) ImGui::Spacing();
		ImGui::Text(u8"音量　");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		bool Input_volume = ImGui::DragFloat(u8"##音量", &Volume, 0.01f, 0.0f, 5.0f, "%.2f");
		ImGui::SameLine();
		bool slide_volume = ImGui::SliderFloat("##音量", &Volume, 0.0f, 5.0f, "%.2f");
		if (Input_volume || slide_volume)
		{
			if (Effect_Instance)
			{
				if (Effect_Instance->GetState() == SoundState::PLAYING)
				{
					Set_Volume(Volume);
				}
			}
		}

		ImGui::Text(u8"ピッチ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		bool Input_pitch = ImGui::DragFloat(u8"##ピッチ", &Pitch, 0.01f, -1.0f, 1.0f, "%.2f");
		ImGui::SameLine();
		bool slide_pitch = ImGui::SliderFloat("##ピッチ", &Pitch, -1.0f, 1.0f, "%.2f");
		if (Input_pitch || slide_pitch)
		{
			if (Effect_Instance)
			{
				if (Effect_Instance->GetState() == SoundState::PLAYING)
				{
					Set_Pitch(Pitch);
				}
			}
		}

		for (int i = 0; i < 5; ++i) ImGui::Spacing();
		ImGui::Checkbox(u8"アクティブ時の自動再生", &Play_On_Awake);
		for (int i = 0; i < 5; ++i) ImGui::Spacing();
		ImGui::Checkbox(u8"ループ再生", &Loop);

		for (int i = 0; i < 5; ++i) ImGui::Spacing();
		if (Effect_Instance)
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