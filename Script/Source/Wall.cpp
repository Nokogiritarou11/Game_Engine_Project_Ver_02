#include "Wall.h"
#include "Wave_Manager.h"
using namespace std;
using namespace BeastEngine;

void Wall::Awake()
{
	collider = Get_Component<Collider>();
	move_se = Get_Component<AudioSource>();
	wall_obj = transform->Find("Wall_Object");

	const function<void()> callback_up = [this]() {this->Wall_Up(); };
	const function<void()> callback_down = [this]() {this->Wall_Down(); };
	const auto& manager_parent = GameObject::Find("Wave_Manager").lock()->transform;
	const auto& wall_01 = manager_parent->Find("Wave_01").lock()->Get_Component<Wave_Manager>();
	const auto& wall_02 = manager_parent->Find("Wave_02").lock()->Get_Component<Wave_Manager>();
	wall_01->Add_Callback_Start(callback_up);
	wall_01->Add_Callback_End(callback_down);
	wall_02->Add_Callback_Start(callback_up);
	wall_02->Add_Callback_End(callback_down);
}

void Wall::Update()
{
	if (state != Wall_State::Stay)
	{
		const auto& wall = wall_obj.lock();
		fade_timer += Time::delta_time;
		if (state == Wall_State::Up)
		{
			if (const float rate = fade_timer / fade_time; rate < 1)
			{
				wall->Set_Local_Position(Vector3::Lerp(start_pos, end_pos, rate));
			}
			else
			{
				wall->Set_Local_Position(end_pos);
				state = Wall_State::Stay;
			}
		}
		else if (state == Wall_State::Down)
		{
			if (const float rate = fade_timer / fade_time; rate < 1)
			{
				wall->Set_Local_Position(Vector3::Lerp(end_pos, start_pos, rate));
			}
			else
			{
				wall->Set_Local_Position(start_pos);
				state = Wall_State::Stay;
			}
		}
	}
}

void Wall::Wall_Up()
{
	state = Wall_State::Up;
	fade_timer = 0;
	const auto& se = move_se.lock();
	se->Play_OneShot(se->Get_Volume(), se->Get_Pitch());
	collider.lock()->Set_Enabled(true);
}

void Wall::Wall_Down()
{
	state = Wall_State::Down;
	fade_timer = 0;
	const auto& se = move_se.lock();
	se->Play_OneShot(se->Get_Volume(), se->Get_Pitch());
	collider.lock()->Set_Enabled(false);
}

bool Wall::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Wall", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"遷移時間", "##fade_time", &fade_time, window_center);
		ImGui::LeftText_DragFloat3(u8"壁開始位置", "##start_pos", start_pos, window_center);
		ImGui::LeftText_DragFloat3(u8"壁終了位置", "##end_pos", end_pos, window_center);
	}
	return true;
}