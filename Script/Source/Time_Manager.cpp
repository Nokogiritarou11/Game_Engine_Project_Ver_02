#include "Time_Manager.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Time_Manager::Update()
{
	//スロー効果再生中か
	if (is_slow)
	{
		if (active_slow)
		{
			//スロー効果中なのでdelta_timeに逆数を掛けて実際の時間を算出する
			slow_timer -= Time::delta_time * inverse_time_scale;
			if (slow_timer < 0.0f)
			{
				//スロー効果の終了
				Time::time_scale = 1;
				is_slow = false;
				active_slow = false;
			}
		}
		else
		{
			//効果発生ディレイ中
			//ディレイ時間が経過するまで処理を延期する
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
	//重複防止
	if (!is_slow)
	{
		//スロー効果のパラメータを設定する
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
		ImGui::Text(u8"設定できるパラメータはありません");
	}
	return true;
}