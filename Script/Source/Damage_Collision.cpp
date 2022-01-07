#include "Damage_Collision.h"
#include "Character_Parameter.h"
#include "Character_Hit_Stop_Manager.h"
#include "Engine.h"
#include "Editor.h"

using namespace std;
using namespace BeastEngine;

void Damage_Collision::Awake()
{
	//�����o�|�C���^�̎擾
	hit_stop_manager = root_transform.lock()->Get_Component<Character_Hit_Stop_Manager>();
	hit_transform = transform->Get_Child(0);
	animator = root_transform.lock()->Get_Component<Animator>();
}

void Damage_Collision::OnTrigger_Enter(Collision& collision)
{
	//�_���[�W���󂯂鎖���ł���I�u�W�F�N�g������
	if (const auto& hit = collision.gameobject->Get_Component<Interface_Character_Damageable>())
	{
		//�_���[�W��^����
		if (hit->Take_Damage(static_pointer_cast<Damage_Collision>(shared_from_this())))
		{
			//�U���������̓q�b�g�X�g�b�v
			hit_stop_manager.lock()->Start_Hit_Stop(hit_stop_time, stop_particle);
		}
	}
}

bool Damage_Collision::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Damage_Collision", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::Text(u8"�ʒu����p�e");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);

		string label_parent = u8"���ݒ� (�����Ƀh���b�O)";
		if (const auto& p = root_transform.lock())
		{
			label_parent = p->gameobject->name;
		}
		ImGui::InputText("##Item", &label_parent, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget())
		{
			if (const auto& drag = Engine::editor->Get_Drag_Object())
			{
				root_transform = drag->transform;
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::TreeNode(u8"��~�p�[�e�B�N�����X�g"))
		{
			int size = static_cast<int>(stop_particle.size());

			ImGui::Text(u8"�v�f��");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				stop_particle.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);
				ImGui::Text(u8"�v�f %d", i);
				ImGui::SameLine(window_center);
				ImGui::SetNextItemWidth(-FLT_MIN);

				string label_particle = u8"���ݒ� (�����Ƀh���b�O)";
				if (const auto& p = stop_particle[i].lock())
				{
					if (p->gameobject)
					{
						label_particle = p->gameobject->name;
					}
					else
					{
						stop_particle[i].reset();
					}
				}
				ImGui::InputText("##Item", &label_particle, ImGuiInputTextFlags_ReadOnly);

				if (ImGui::BeginDragDropTarget())
				{
					if (const auto& drag = Engine::editor->Get_Drag_Object())
					{
						if (const auto& p = drag->Get_Component<Particle>())
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

		ImGui::LeftText_DragInt(u8"HP�_���[�W", "##damage_hp", &damage_hp, window_center);
		ImGui::LeftText_DragInt(u8"�X�^���_���[�W", "##damage_stun", &damage_stun, window_center);

		const char* types[] = { "Front", "Back", "Right", "Left", "Push", "Knock_Down", "Up", "Down" };
		int select = static_cast<int>(damage_type);
		if (ImGui::LeftText_Combo(u8"�_���[�W�^�C�v", "##damage_type", &select, types, 8, window_center))
		{
			damage_type = static_cast<Damage_Type>(select);
		}

		ImGui::LeftText_Checkbox(u8"�p���B�\", "##can_parry", &can_parry, window_center);
		ImGui::LeftText_DragFloat(u8"�q�b�g�X�g�b�v����", "##hit_stop_time", &hit_stop_time, window_center);
		ImGui::LeftText_InputInt(u8"�J�����V�F�C�N��", "##shake_count", &shake_count, window_center);
		ImGui::LeftText_DragFloat(u8"�J�����V�F�C�N���x", "##shake_power", &shake_power, window_center);
		ImGui::LeftText_InputText(u8"�q�b�g�G�t�F�N�g�L�[", "##hit_particle_key", &hit_particle_key, window_center);
	}
	return true;
}