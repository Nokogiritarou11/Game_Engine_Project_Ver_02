#pragma once

namespace BeastEngine
{
	class Time
	{
	public:
		static float delta_time; //フレーム間の時間(秒)
		static float time_scale; //delta_timeにかける補正値
	private:
	};
}