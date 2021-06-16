#pragma once
#include <algorithm>
#include <random>
#include <functional>

namespace BeastEngine
{
	struct Mathf
	{
		//startとendをtで線形補間する
		static float Lerp(float start, float end, float t)
		{
			return (1 - t) * start + t * end;
		}

		//指定した確率(％単位)でTrueを返す
		static bool Probability(float fPercent)
		{
			fPercent /= 100;
			if ((float)std::rand() / RAND_MAX < fPercent)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		//minとmaxの間でランダムな値を返す(maxは含まれないことに注意)
		static float Random_Range(float min, float max)
		{
			float f = min + static_cast <float> (std::rand()) / (static_cast <float> (RAND_MAX / (max - min)));
			return f;
		}

		//ValueをMinとMaxの間にクランプする
		static float Clamp(float Value, float Min, float Max)
		{
			float f = (std::min)((std::max)(Value, Min), Max);
			return f;
		}

		//Float誤差を考慮し同値かどうかを返す
		static bool Approximately(float x, float y, const float epsilon = FLT_EPSILON)
		{
			if (std::fabsf(x - y) < epsilon)return true;
			return false;
		}
	};
}