#include "Damage_Collision.h"
#include "Character_Parameter.h"
#include "Character_Hit_Stop_Manager.h"
#include "Object_Pool.h"
#include "Engine.h"
#include "Editor.h"

using namespace std;
using namespace BeastEngine;

void Damage_Collision::Awake()
{
	hit_stop_manager = root_transform.lock()->Get_Component<Character_Hit_Stop_Manager>();
	pool = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Object_Pool>();
	hit_transform = transform->Get_Child(0);
}

void Damage_Collision::OnTrigger_Enter(Collision& collision)
{
	if (auto& hit = collision.gameobject->Get_Component<Interface_Character_Damageable>())
	{
		hit->Take_Damage(damage_hp, damage_stun, root_transform.lock()->Get_Position(), damage_type);
		hit_stop_manager.lock()->Start_Hit_Stop(stop_particle);

		Vector3 pos = collision.transform->Get_Position();
		pos.y += 1;
		pool.lock()->Instance_In_Pool(hit_particle_key, pos, hit_transform.lock()->Get_Rotation());
	}
}

bool Damage_Collision::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Damage_Collision", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::Text(u8"位置判定用親");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);

		string label = u8"未設定 (ここにドラッグ)";
		if (auto& p = root_transform.lock())
		{
			label = p->gameobject->name;
		}
		ImGui::InputText("##Item", &label, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget())
		{
			if (const auto& drag = Engine::editor->Get_Drag_Object())
			{
				root_transform = drag->transform;
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::TreeNode(u8"停止パーティクルリスト"))
		{
			int size = static_cast<int>(stop_particle.size());

			ImGui::Text(u8"要素数");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				stop_particle.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);
				ImGui::Text(u8"要素 %d", i);
				ImGui::SameLine(window_center);
				ImGui::SetNextItemWidth(-FLT_MIN);

				string label = u8"未設定 (ここにドラッグ)";
				if (auto& p = stop_particle[i].lock())
				{
					if (p->gameobject)
					{
						label = p->gameobject->name;
					}
					else
					{
						stop_particle[i].reset();
					}
				}
				ImGui::InputText("##Item", &label, ImGuiInputTextFlags_ReadOnly);

				if (ImGui::BeginDragDropTarget())
				{
					if (const auto& drag = Engine::editor->Get_Drag_Object())
					{
						if (auto& p = drag->Get_Component<Particle>())
						{
							stop_particle[i] = p;
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		ImGui::LeftText_DragInt(u8"HPダメージ", "##damage_hp", &damage_hp, window_center);
		ImGui::LeftText_DragInt(u8"スタンダメージ", "##damage_stun", &damage_stun, window_center);

		const char* types[] = { "Front", "Back", "Right", "Left", "Knock_Down", "Up", "Down" };
		int select = static_cast<int>(damage_type);
		if (ImGui::LeftText_Combo(u8"ダメージタイプ", "##damage_type", &select, types, 7, window_center))
		{
			damage_type = static_cast<Damage_Type>(select);
		}

		ImGui::LeftText_InputText(u8"ヒットエフェクトキー", "##hit_particle_key", &hit_particle_key, window_center);
	}
	return true;
}