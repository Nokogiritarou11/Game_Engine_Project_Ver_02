#include "Character_Sounds.h"
#include "Object_Pool.h"
#include "Engine.h"
#include "Editor.h"

using namespace std;
using namespace BeastEngine;

void Character_Sounds::Awake()
{
	//メンバポインタの取得
	animator = Get_Component<Animator>();
}

void Character_Sounds::Update()
{
	const auto& anim = animator.lock();

	//Animatorからアクティブにするパーティクルの情報を受け取る
	if (const int number = anim->Get_Int("Sound_Number"); number >= 0)
	{
		//鳴らしたあとAnimatorのパラメータをリセットする
		const auto& sound = sound_list[number].lock();
		sound->Play_OneShot(sound->Get_Volume(), sound->Get_Pitch());
		anim->Set_Int("Sound_Number", -1);
	}
}

bool Character_Sounds::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Sounds", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		if (ImGui::TreeNode(u8"効果音リスト"))
		{
			int size = static_cast<int>(sound_list.size());

			ImGui::Text(u8"要素数");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				sound_list.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);
				ImGui::Text(u8"要素 %d", i);
				ImGui::SameLine(window_center);
				ImGui::SetNextItemWidth(-FLT_MIN);

				string label = u8"未設定 (ここにドラッグ)";
				if (const auto& p = sound_list[i].lock())
				{
					label = p->gameobject->name;
				}
				ImGui::InputText("##Item", &label, ImGuiInputTextFlags_ReadOnly);

				if (ImGui::BeginDragDropTarget())
				{
					if (const auto& drag = Engine::editor->Get_Drag_Object())
					{
						if (const auto& audio = drag->Get_Component<AudioSource>())
						{
							sound_list[i] = audio;
						}
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