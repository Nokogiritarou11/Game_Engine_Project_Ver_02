#include "Character_Ground_Checker.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Character_Ground_Checker::Awake()
{
	//�����o�|�C���^�̎擾
	parameter = Get_Component<Character_Parameter>();
	animator = Get_Component<Animator>();
}

void Character_Ground_Checker::Update()
{
	//���g�̌��_(����)���班����̍��W�����C�̌��_�Ƃ���
	Vector3 from = transform->Get_Position();
	from.y += 0.1f;
	Vector3 to = from;
	to.y -= ray_distance;

	Vector3 hit_pos;
	//���C�L���X�g�̃q�b�g����
	if (const bool ground = Physics::Raycast(from, to, hit_pos); ground != ground_old)
	{
		//�q�b�g�����ꍇ�͐ڒn���Ă���Ɣ��f����
		ground_old = ground;
		parameter.lock()->is_ground = ground;
		animator.lock()->Set_Bool("Is_Ground", ground);
	}
}

bool Character_Ground_Checker::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Ground_Checker", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"���C�̋���", "##Ray_Distance", &ray_distance, window_center);
	}
	return true;
}