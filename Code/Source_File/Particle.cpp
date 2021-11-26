#include "Particle.h"
#include "GameObject.h"
#include "Transform.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Particle_Manager.h"
#include "System_Function.h"
#include "Debug.h"
#include "Asset_Manager.h"
using namespace std;
using namespace BeastEngine;

Particle::~Particle()
{
	if (effect != nullptr)
	{
		// インスタンスが存在しているか確認して
		if (Engine::particle_manager->manager->Exists(handle))
		{
			// 再生を停止する
			Engine::particle_manager->manager->StopEffect(handle);
		}
		effect = nullptr;
	}
}

void Particle::Initialize(const shared_ptr<GameObject>& obj)
{
	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;
	if (file_path != "")
	{
		Set_Particle(file_path, file_name);
	}
	Set_Active(gameobject->Get_Active_In_Hierarchy());
}

void Particle::Set_Particle(const string& filepath, const string& filename)
{
	if (effect != nullptr)
	{
		// インスタンスが存在しているか確認して
		if (Engine::particle_manager->manager->Exists(handle))
		{
			// 再生を停止する
			Engine::particle_manager->manager->StopEffect(handle);
		}
		effect = nullptr;
		handle = -1;
	}
	file_name = filename;
	file_path = filepath;
	const string full_path = filepath + filename;

	if (const auto it = Engine::particle_manager->effect_cache.find(full_path); it != Engine::particle_manager->effect_cache.end())
	{
		effect = it->second;
	}
	else
	{
		// EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
		const char* utf8_filename = full_path.c_str();
		char16_t utf16_filename[MAX_PATH];
		Effekseer::ConvertUtf8ToUtf16(utf16_filename, MAX_PATH, utf8_filename);
		effect = Effekseer::Effect::Create(Engine::particle_manager->manager, static_cast<EFK_CHAR*>(utf16_filename));
		if (effect != nullptr)
		{
			Engine::particle_manager->effect_cache.insert(make_pair(full_path, effect));
		}
	}
	handle = -1;
	is_called = false;
	Set_Active(gameobject->Get_Active_In_Hierarchy());
}

void Particle::Set_Active(const bool value)
{
	if (value)
	{
		if (!is_called)
		{
			Engine::particle_manager->Add(static_pointer_cast<Particle>(shared_from_this()));
			is_called = true;
		}

		//自動再生がオンなら鳴らす
		if (Engine::scene_manager->run)
		{
			if (play_on_awake)
			{
				Play();
			}
			else
			{
				Stop();
			}
		}
	}
	else
	{
		if (Engine::scene_manager->run)
		{
			Stop();
		}
	}
}

void Particle::Play()
{
	if (effect != nullptr)
	{
		if (gameobject->Get_Active_In_Hierarchy())
		{
			if (!Engine::particle_manager->manager->Exists(handle))
			{
				// マネージャーにエフェクトを渡すことでインスタンスが生成され、操作ハンドルが返される
				handle = Engine::particle_manager->manager->Play(effect, Effekseer::Vector3D(0.0f, 0.0f, 0.0f));
				is_playing = true;
			}
			else
			{
				if (Engine::particle_manager->manager->GetPaused(handle))
				{
					Engine::particle_manager->manager->SetPaused(handle, false);
					is_playing = true;
				}
			}
		}
	}
}

void Particle::Pause() const
{
	if (effect != nullptr)
	{
		if (gameobject->Get_Active_In_Hierarchy())
		{
			// インスタンスが存在しているか確認して
			if (Engine::particle_manager->manager->Exists(handle))
			{
				// 再生を一時停止する
				Engine::particle_manager->manager->SetPaused(handle, true);
			}
		}
	}
}

void Particle::Stop()
{
	if (effect != nullptr)
	{
		if (gameobject->Get_Active_In_Hierarchy())
		{
			// インスタンスが存在しているか確認して
			if (Engine::particle_manager->manager->Exists(handle))
			{
				// 再生を停止する
				Engine::particle_manager->manager->StopEffect(handle);
				is_playing = false;
			}
		}
	}
}

bool Particle::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	const bool open = ImGui::CollapsingHeader("Particle");

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Particle_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Component>(shared_from_this()));
			removed = true;
		}
		ImGui::EndPopup();
	}
	if (removed)
	{
		return false;
	}

	if (open)
	{
		ImGui::Text(u8"セット中::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());
		const float window_width = ImGui::GetWindowContentRegionWidth();

		ImGui::SameLine(window_width - 25.0f);
		if (ImGui::Button(u8"選択"))
		{
			if (const string path = System_Function::Get_Open_File_Name("", "\\Assets\\Effect"); path != "")
			{
				const int path_i = path.find_last_of("\\") + 1;//7
				const int ext_i = path.find_last_of(".");//10
				const string path_name = path.substr(0, path_i); //ファイルまでのディレクトリ
				const string ext_name = path.substr(ext_i, path.size() - ext_i); //拡張子
				const string file_name = path.substr(path_i, ext_i - path_i); //ファイル名
				const string name = file_name + ext_name;
				if (ext_name == ".efkefc" || ext_name == ".efk")
				{
					Set_Particle(path_name, name);
				}
				else
				{
					Debug::Log("ファイル形式が対応していません");
				}
			}
			else
			{
				Debug::Log("ファイルを開けませんでした");
			}
		}

		ImGui::Text(u8"再生速度");
		ImGui::SameLine(window_width * 0.5f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat("##speed", &play_speed, 0.01f, 0.0f, FLT_MAX);

		ImGui::Text(u8"アクティブ時の自動再生");
		ImGui::SameLine(window_width * 0.5f);
		ImGui::Checkbox("##play_awake", &play_on_awake);

		ImGui::Dummy(ImVec2(0, 3));
		if (effect != nullptr)
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