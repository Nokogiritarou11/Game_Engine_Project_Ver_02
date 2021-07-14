#include "Animator.h"
#include "Engine.h"
#include "Animator_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animation_Clip.h"
#include "Animator_State_Machine.h"
#include "Animator_Controller.h"
#include "Time_Engine.h"
#include "Include_ImGui.h"
#include "System_Function.h"
#include "Engine.h"
#include "Asset_Manager.h"
using namespace std;
using namespace BeastEngine;

void Animator::Initialize(shared_ptr<GameObject> obj)
{
	Engine::asset_manager->Registration_Asset(shared_from_this());
	gameobject = obj;
	transform = obj->transform;
	Engine::animator_manager->Add(static_pointer_cast<Animator>(shared_from_this()));

	if (!controller_path.empty())
	{
		controller = Animator_Controller::Load_Animator_Controller(controller_path);
		controller->Initialize(transform);
	}
}

void Animator::Set_Int(string key, int value)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_int = value;
	}
}
void Animator::Set_Float(string key, float value)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_float = value;
	}
}
void Animator::Set_Bool(string key, bool value)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_bool = value;
	}
}
void Animator::Set_Trigger(string key)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_bool = true;
	}
}
void Animator::Reset_Trigger(string key)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_bool = false;
	}
}

void Animator::Update()
{
	if (!playing) return;

	controller->Update();
	for (auto& data : controller->animation_data)
	{
		Animator_Controller::Animation_Target& anim = data.second;
		if (anim.humanoid_rig == Humanoid_Rig::None)
		{
			shared_ptr<Transform> target = anim.target.lock();
			target->Set_Local_Position(anim.position);
			target->Set_Local_Rotation(anim.rotation);
			target->Set_Local_Scale(anim.scale);
		}
		else
		{
			//TODO:Avater完成後に追加
		}
	}
}

// アニメーション再生
void Animator::Play()
{
	playing = true;
}

void Animator::Stop()
{
	playing = false;
}

void Animator::Pause()
{
	playing = false;
}

bool Animator::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Animator_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Animator>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
		if (!removed)
		{
			return false;
		}
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	static bool enable;
	enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{
		if (controller)
		{
			static string controller_name;
			controller_name = controller->name + ".controller";
			ImGui::Text(controller_name.c_str());
		}
		else
		{
			ImGui::Text(u8"アニメーターコントローラーが登録されていません");
		}
		ImGui::Spacing();
		if (ImGui::Button(u8"新規作成"))
		{
			controller = Animator_Controller::Create_New_Controller();
			if (controller)
			{
				controller_path = controller->save_path;
				controller->Initialize(transform);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"読み込む"))
		{
			controller = Animator_Controller::Load_Animator_Controller();
			if (controller)
			{
				controller_path = controller->save_path;
				controller->Initialize(transform);
			}
		}
	}
	return true;
}