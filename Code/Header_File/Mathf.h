#pragma once
#include <algorithm>
#include <random>
#include <functional>

struct Mathf
{
	static float Lerp(float start, float end, float t)
	{
		return (1 - t) * start + t * end;
	}

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

	static float Random_Range(float min, float max)
	{
		float f = min + static_cast <float> (std::rand()) / (static_cast <float> (RAND_MAX / (max - min)));
		return f;
	}

	static float Clamp(float Value, float Min, float Max)
	{
		float f = (std::min)((std::max)(Value, Min), Max);
		return f;
	}

	static bool Approximately(float x, float y, const float epsilon = FLT_EPSILON)
	{
		if (std::fabsf(x - y) < epsilon)return true;
		return false;
	}
};