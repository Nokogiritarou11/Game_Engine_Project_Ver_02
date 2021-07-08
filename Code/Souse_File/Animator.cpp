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

	if (animator_controller)
	{
		animator_controller->Initialize(transform);
	}
}

void Animator::Set_Int(string key, int value)
{

}
void Animator::Set_Float(string key, float value)
{

}
void Animator::Set_Bool(string key, bool value)
{

}
void Animator::Set_Trigger(string key)
{

}
void Animator::Reset_Trigger(string key)
{

}

void Animator::Update()
{
	if (!playing) return;

	animator_controller->Update();
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
		if (animator_controller)
		{
			if (ImGui::Button(ICON_FA_PLAY))
			{
				Play();
			}

			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_STOP))
			{
				Stop();
			}

			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PAUSE))
			{
				Pause();
			}
		}
		else
		{
			ImGui::Text(u8"アニメーターコントローラーが登録されていません");
		}
	}
	return true;
}