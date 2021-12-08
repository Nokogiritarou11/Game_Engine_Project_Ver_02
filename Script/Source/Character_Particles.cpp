#include "Character_Particles.h"
#include "Object_Pool.h"
#include "Engine.h"
#include "Editor.h"

using namespace std;
using namespace BeastEngine;

void Character_Particles::Awake()
{
	//�����o�|�C���^�̎擾
	animator = Get_Component<Animator>();
	pool = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Object_Pool>();
}

void Character_Particles::Update()
{
	const auto& anim = animator.lock();

	//Animator����A�N�e�B�u�ɂ���p�[�e�B�N���̏����󂯎��
	int number = anim->Get_Int("Particle_Number");
	if (number >= 0)
	{
		//�A�N�e�B�u����������Animator�̃p�����[�^�����Z�b�g����
		particle_list[number].lock()->Set_Active(true);
		anim->Set_Int("Particle_Number", -1);
	}

	//Animator����A�N�e�B�u�ɂ���R���C�_�[�̏����󂯎��
	number = anim->Get_Int("Collider_Number");
	if (number >= 0)
	{
		//�A�N�e�B�u����������Animator�̃p�����[�^�����Z�b�g����
		collider_list[number].lock()->Set_Active(true);
		anim->Set_Int("Collider_Number", -1);
	}

	//Animator����C���X�^���X����v�[���̃L�[�����󂯎��
	number = anim->Get_Int("Instance_Number");
	if (number >= 0)
	{
		//�C���X�^���X��������Animator�̃p�����[�^�����Z�b�g����
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

		if (ImGui::TreeNode(u8"�p�[�e�B�N�����X�g"))
		{
			int size = static_cast<int>(particle_list.size());

			ImGui::Text(u8"�v�f��");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				particle_list.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);
				ImGui::Text(u8"�v�f %d", i);
				ImGui::SameLine(window_center);
				ImGui::SetNextItemWidth(-FLT_MIN);

				string label = u8"���ݒ� (�����Ƀh���b�O)";
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

		if (ImGui::TreeNode(u8"�R���C�_�[���X�g"))
		{
			int size = static_cast<int>(collider_list.size());

			ImGui::Text(u8"�v�f��");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				collider_list.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);
				ImGui::Text(u8"�v�f %d", i);
				ImGui::SameLine(window_center);
				ImGui::SetNextItemWidth(-FLT_MIN);

				string label = u8"���ݒ� (�����Ƀh���b�O)";
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

		if (ImGui::TreeNode(u8"�v�[���C���X�^���X���X�g"))
		{
			int size = static_cast<int>(instance_list.size());

			ImGui::Text(u8"�v�f��");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				instance_list.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);
				const string label = u8"�v�f" + to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					auto& data = instance_list[i];
					ImGui::LeftText_InputText(u8"�L�[", "##key", &data.key, window_center);
					ImGui::LeftText_DragFloat3(u8"���[�J�����W", "##position", data.position, window_center);
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	return true;
}