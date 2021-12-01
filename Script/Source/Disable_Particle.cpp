#include "Disable_Particle.h"

using namespace std;
using namespace BeastEngine;

void Disable_Particle::Awake()
{
	//�����o�|�C���^�̎擾
	particle = Get_Component<Particle>();
}

void Disable_Particle::Update()
{
	//�p�[�e�B�N���̑��݃`�F�b�N
	if (const auto& p = particle.lock())
	{
		//�p�[�e�B�N�����Đ��I�����Ă����ꍇ
		if (!p->Get_Is_Playing())
		{
			//�e�q����������ꍇ
			if (remove_parent)
			{
				transform->Set_Parent(nullptr);
			}
			//�Q�[���I�u�W�F�N�g���A�N�e�B�u��
			gameobject->Set_Active(false);
		}
	}
}

bool Disable_Particle::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Disable_Particle", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_Checkbox("remove_parent", "##remove_parent", &remove_parent, window_center);
	}
	return true;
}