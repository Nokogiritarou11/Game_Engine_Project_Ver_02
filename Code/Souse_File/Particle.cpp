#include "Particle.h"
#include "GameObject.h"
#include "Transform.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Particle_Manager.h"
#include "System_Function.h"
#include "Debug.h"
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

void Particle::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	if (file_path != "")
	{
		Set_Particle(file_path.c_str(), file_name.c_str());
	}
	SetActive(gameObject->activeInHierarchy());
}

void Particle::Set_Particle(const char* filepath, const char* filename)
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
	// EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
	const string fullpath = (string)filepath + (string)filename;

	auto it = Engine::particle_manager->effect_cache.find(fullpath);
	if (it != Engine::particle_manager->effect_cache.end())
	{
		effect = it->second;
	}
	else
	{
		const char* utf8Filename = fullpath.c_str();
		char16_t utf16Filename[MAX_PATH];
		Effekseer::ConvertUtf8ToUtf16(utf16Filename, MAX_PATH, utf8Filename);
		effect = Effekseer::Effect::Create(Engine::particle_manager->manager, (EFK_CHAR*)utf16Filename);
		if (effect != nullptr)
		{
			Engine::particle_manager->effect_cache.insert(make_pair(fullpath, effect));
		}
	}
	handle = -1;
	IsCalled = false;
	SetActive(gameObject->activeInHierarchy());
}

void Particle::SetActive(bool value)
{
	if (value)
	{
		if (!IsCalled)
		{
			Engine::particle_manager->Add(static_pointer_cast<Particle>(shared_from_this()));
			IsCalled = true;
		}
		if (Play_On_Awake)
		{
			if (Engine::scene_manager->Run)
			{
				Play();
			}
		}
		else
		{
			if (Engine::scene_manager->Run)
			{
				Stop();
			}
		}
		Disable_flg = false;
	}
	else
	{

		if (Engine::scene_manager->Run)
		{
			Stop();
		}
	}
}

void Particle::Play()
{
	if (effect != nullptr)
	{
		if (gameObject->activeInHierarchy())
		{
			if (!Engine::particle_manager->manager->Exists(handle))
			{
				// マネージャーにエフェクトを渡すことでインスタンスが生成され、操作ハンドルが返される
				handle = Engine::particle_manager->manager->Play(effect, Effekseer::Vector3D(0.0f, 0.0f, 0.0f));
			}
			else
			{
				if (Engine::particle_manager->manager->GetPaused(handle))
				{
					Engine::particle_manager->manager->SetPaused(handle, false);
				}
			}
		}
	}
}

void Particle::Pause()
{
	if (effect != nullptr)
	{
		if (gameObject->activeInHierarchy())
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
		if (gameObject->activeInHierarchy())
		{
			// インスタンスが存在しているか確認して
			if (Engine::particle_manager->manager->Exists(handle))
			{
				// 再生を停止する
				Engine::particle_manager->manager->StopEffect(handle);
			}
		}
	}
}

bool Particle::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Particle");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Particle_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Particle>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
	}
	if (!removed)
	{
		return false;
	}

	if (open)
	{
		ImGui::Text(u8"現在のパーティクル::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());
		if (ImGui::Button(u8"パーティクルを選択"))
		{
			string path = System_Function::Get_Open_File_Name();
			//Debug::Log(path);
			if (path != "")
			{
				int path_i = path.find_last_of("\\") + 1;//7
				int ext_i = path.find_last_of(".");//10
				string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
				string extname = path.substr(ext_i, path.size() - ext_i); //拡張子
				string filename = path.substr(path_i, ext_i - path_i); //ファイル名
				string name = filename + extname;
				if (extname == ".efkefc" || extname == ".efk")
				{
					Set_Particle(pathname.c_str(), name.c_str());
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
		for (int i = 0; i < 5; ++i) ImGui::Spacing();

		ImGui::DragFloat(u8"再生速度", &Play_Speed, 0.01f, 0.0f, FLT_MAX);
		for (int i = 0; i < 5; ++i) ImGui::Spacing();
		ImGui::Checkbox(u8"アクティブ時の自動再生", &Play_On_Awake);
		for (int i = 0; i < 5; ++i) ImGui::Spacing();
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