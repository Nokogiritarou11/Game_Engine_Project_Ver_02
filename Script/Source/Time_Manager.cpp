#include "Time_Manager.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Time_Manager::Update()
{
	if (is_slow)
	{
		if (active_slow)
		{
			slow_timer -= Time::delta_time * inverse_time_scale;
			if (slow_timer < 0.0f)
			{
				Time::time_scale = 1;
				is_slow = false;
				active_slow = false;
			}
		}
		else
		{
			delay_timer -= Time::delta_time;
			if (delay_timer <= 0)
			{
				Time::time_scale = slow_scale;
				active_slow = true;
			}
		}
	}
}

void Time_Manager::Start_Time_Slow(const float delay_time, const float slow_time, const float slow_speed)
{
	if (!is_slow)
	{
		slow_timer = slow_time;
		delay_timer = delay_time;
		inverse_time_scale = 1 / slow_speed;
		slow_scale = slow_speed;
		is_slow = true;
		active_slow = false;
	}
}

bool Time_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Time_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"Ý’è‚Å‚«‚éƒpƒ‰ƒ[ƒ^‚Í‚ ‚è‚Ü‚¹‚ñ");
	}
	return true;
}