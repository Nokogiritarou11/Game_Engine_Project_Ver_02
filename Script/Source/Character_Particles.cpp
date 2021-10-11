#include "Character_Particles.h"
#include "Engine.h"
#include "Editor.h"

using namespace std;
using namespace BeastEngine;

void Character_Particles::Awake()
{
	animator = Get_Component<Animator>();
}

void Character_Particles::Update()
{
	const auto& anim = animator.lock();

	int number = anim->Get_Int("Particle_Number");
	if (number >= 0)
	{
		particle_list[number].lock()->Set_Active(true);
		anim->Set_Int("Particle_Number", -1);
	}

	number = anim->Get_Int("Collider_Number");
	if (number >= 0)
	{
		collider_list[number].lock()->Set_Active(true);
		anim->Set_Int("Collider_Number", -1);
	}
}

bool Character_Particles::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Particles", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		if (ImGui::TreeNode(u8"パーティクルリスト"))
		{
			int size = static_cast<int>(particle_list.size());

			ImGui::Text(u8"要素数");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				particle_list.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);
				ImGui::Text(u8"要素 %d", i);
				ImGui::SameLine(window_center);
				ImGui::SetNextItemWidth(-FLT_MIN);

				string label = u8"未設定 (ここにドラッグ)";
				if (const auto& p = particle_list[i].lock())
				{
					label = p->name;
				}
				ImGui::InputText("##Item", &label, ImGuiInputTextFlags_ReadOnly);

				if (ImGui::BeginDragDropTarget())
				{
					if (const auto& drag = Engine::editor->Get_Drag_Object())
					{
						particle_list[i] = drag;
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode(u8"コライダーリスト"))
		{
			int size = static_cast<int>(collider_list.size());

			ImGui::Text(u8"要素数");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				collider_list.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);
				ImGui::Text(u8"要素 %d", i);
				ImGui::SameLine(window_center);
				ImGui::SetNextItemWidth(-FLT_MIN);

				string label = u8"未設定 (ここにドラッグ)";
				if (const auto& p = collider_list[i].lock())
				{
					label = p->name;
				}
				ImGui::InputText("##Item", &label, ImGuiInputTextFlags_ReadOnly);

				if (ImGui::BeginDragDropTarget())
				{
					if (const auto& drag = Engine::editor->Get_Drag_Object())
					{
						collider_list[i] = drag;
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	return true;
}