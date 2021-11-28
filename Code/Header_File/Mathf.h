#pragma once
#include <algorithm>
#include <random>

namespace BeastEngine
{
	//自作算術関数
	struct Mathf
	{
	private:
		static std::mt19937 gen;
		static std::random_device rd;

	public:
		static void Reset_Random_Seed()
		{
			gen = std::mt19937(rd());
		}

		//startとendをtで線形補間する
		static float Lerp(const float start, const float end, const float t)
		{
			return (1 - t) * start + t * end;
		}

		//指定した確率(％単位)でTrueを返す
		static bool Probability(const float f_percent)
		{
			if (const std::uniform_real_distribution<> dist(0, 100); dist(gen) < f_percent)
			{
				return true;
			}

			return false;
		}

		//minとmaxの間でランダムな値を返す
		static float Random_Range(const float min, const float max)
		{
			const std::uniform_real_distribution<> dist(min, max);
			return static_cast<float>(dist(gen));
		}

		//minとmaxの間でランダムな値を返す
		static int Random_Range(const int min, const int max)
		{
			const std::uniform_int_distribution<> dist(min, max);
			return dist(gen);
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