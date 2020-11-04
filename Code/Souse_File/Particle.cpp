#include "Particle.h"
#include "GameObject.h"
#include "Transform.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Particle_Manager.h"
#include "System_Function.h"
#include "Debug.h"
using namespace std;

Particle::~Particle()
{
	if (effect != nullptr)
	{
		// �C���X�^���X�����݂��Ă��邩�m�F����
		if (Engine::particle_manager->manager->Exists(handle))
		{
			// �Đ����~����
			Engine::particle_manager->manager->StopEffect(handle);
		}
		effect = nullptr;
	}
}

void Particle::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	SetActive(gameObject->activeInHierarchy());
	if (file_path != "")
	{
		Set_Particle(file_path.c_str(), file_name.c_str());
	}
}

void Particle::Set_Particle(const char* filepath, const char* filename)
{
	if (effect != nullptr)
	{
		// �C���X�^���X�����݂��Ă��邩�m�F����
		if (Engine::particle_manager->manager->Exists(handle))
		{
			// �Đ����~����
			Engine::particle_manager->manager->StopEffect(handle);
		}
		effect = nullptr;
		handle = -1;
	}
	file_name = filename;
	file_path = filepath;
	// Effekseer��UTF-16�̃t�@�C���p�X�ȊO�͑Ή����Ă��Ȃ����ߕ����R�[�h�ϊ����K�v
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
			Play();
		}
		else
		{
			Stop();
		}
		Disable_flg = false;
	}
	else
	{
		Stop();
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
				// �}�l�[�W���[�ɃG�t�F�N�g��n�����ƂŃC���X�^���X����������A����n���h�����Ԃ����
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
			// �C���X�^���X�����݂��Ă��邩�m�F����
			if (Engine::particle_manager->manager->Exists(handle))
			{
				// �Đ����ꎞ��~����
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
			// �C���X�^���X�����݂��Ă��邩�m�F����
			if (Engine::particle_manager->manager->Exists(handle))
			{
				// �Đ����~����
				Engine::particle_manager->manager->StopEffect(handle);
			}
		}
	}
}

bool Particle::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Particle"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Particle_sub"))
		{
			if (ImGui::Selectable(u8"�R���|�[�l���g���폜"))
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

		ImGui::Text(u8"���݂̃p�[�e�B�N��::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());
		if (ImGui::Button(u8"�p�[�e�B�N����I��"))
		{
			string path = System_Function::Get_Open_File_Name();
			//Debug::Log(path);
			if (path != "")
			{
				int path_i = path.find_last_of("\\") + 1;//7
				int ext_i = path.find_last_of(".");//10
				string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
				string extname = path.substr(ext_i, path.size() - ext_i); //�g���q
				string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
				string name = filename + extname;
				Set_Particle(pathname.c_str(), name.c_str());
			}
			else
			{
				Debug::Log("�t�@�C�����J���܂���ł���");
			}
		}

		if (effect != nullptr)
		{
			ImGui::DragFloat(u8"�Đ����x", &Play_Speed, 0.01f, 0.0f, FLT_MAX);
			ImGui::Checkbox(u8"�A�N�e�B�u���̎����Đ�", &Play_On_Awake);
			if (ImGui::Button(ICON_FA_PLAY))
			{
				Play();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PAUSE))
			{
				Pause();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_STOP))
			{
				Stop();
			}
		}
	}
	return true;
}