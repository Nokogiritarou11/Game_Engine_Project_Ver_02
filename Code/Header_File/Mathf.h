#pragma once
#include <algorithm>
#include <random>
#include <functional>

namespace BeastEngine
{
	struct Mathf
	{
		//startとendをtで線形補間する
		static float Lerp(const float start, const float end, const float t)
		{
			return (1 - t) * start + t * end;
		}

		//指定した確率(％単位)でTrueを返す
		static bool Probability(float f_percent)
		{
			f_percent /= 100;
			if (static_cast<float>(std::rand()) / RAND_MAX < f_percent)
			{
				return true;
			}

			return false;
		}

		//minとmaxの間でランダムな値を返す(maxは含まれないことに注意)
		static float Random_Range(const float min, const float max)
		{
			return min + static_cast <float> (std::rand()) / (RAND_MAX / (max - min));
		}

		//ValueをMinとMaxの間にクランプする
		static float Clamp(const float value, const float min, const float max)
		{
			return (std::min)((std::max)(value, min), max);
		}

		//Float誤差を考慮し同値かどうかを返す
		static bool Approximately(const float x, const float y, const float epsilon = FLT_EPSILON)
		{
			if (std::fabsf(x - y) < epsilon)
			{
				return true;
			}

			return false;
		}
	};
}