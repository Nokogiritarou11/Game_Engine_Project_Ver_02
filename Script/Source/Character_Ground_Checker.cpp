#include "Character_Ground_Checker.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Character_Ground_Checker::Awake()
{
	parameter = Get_Component<Character_Parameter>();
	animator = Get_Component<Animator>();
}

void Character_Ground_Checker::Update()
{
	Vector3 from = transform->Get_Position();
	from.y += 0.1f;
	Vector3 to = from;
	to.y -= ray_distance;

	bool ground = Physics::Raycast(from, to);

	if (ground != ground_old)
	{
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
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"ƒŒƒC‚Ì‹——£", "##Ray_Distance", &ray_distance, window_center);
	}
	return true;
}