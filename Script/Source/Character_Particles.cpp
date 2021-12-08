#include "Character_Particles.h"
#include "Object_Pool.h"
#include "Engine.h"
#include "Editor.h"

using namespace std;
using namespace BeastEngine;

void Character_Particles::Awake()
{
	//メンバポインタの取得
	animator = Get_Component<Animator>();
	pool = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Object_Pool>();
}

void Character_Particles::Update()
{
	const auto& anim = animator.lock();

	//Animatorからアクティブにするパーティクルの情報を受け取る
	int number = anim->Get_Int("Particle_Number");
	if (number >= 0)
	{
		//アクティブ化したあとAnimatorのパラメータをリセットする
		particle_list[number].lock()->Set_Active(true);
		anim->Set_Int("Particle_Number", -1);
	}

	//Animatorからアクティブにするコライダーの情報を受け取る
	number = anim->Get_Int("Collider_Number");
	if (number >= 0)
	{
		//アクティブ化したあとAnimatorのパラメータをリセットする
		collider_list[number].lock()->Set_Active(true);
		anim->Set_Int("Collider_Number", -1);
	}

	//Animatorからインスタンスするプールのキー情報を受け取る
	number = anim->Get_Int("Instance_Number");
	if (number >= 0)
	{
		//インスタンスしたあとAnimatorのパラメータをリセットする
		const auto& data = instance_list[number];
		const Vector3 pos = transform->Get_Position();
		pool.lock()->Instance_In_Pool(data.key, pos + transform->Get_Forward() * data.position.z + transform->Get_Left() * data.position.x + transform->Get_Up() * data.position.y, transform->Get_Rotation());
		anim->Set_Int("Instance_Number", -1);
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

		if (ImGui::TreeNode(u8"プールインスタンスリスト"))
		{
			int size = static_cast<int>(instance_list.size());

			ImGui::Text(u8"要素数");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				instance_list.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);
				const string label = u8"要素" + to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					auto& data = instance_list[i];
					ImGui::LeftText_InputText(u8"キー", "##key", &data.key, window_center);
					ImGui::LeftText_DragFloat3(u8"ローカル座標", "##position", data.position, window_center);
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	return true;
}