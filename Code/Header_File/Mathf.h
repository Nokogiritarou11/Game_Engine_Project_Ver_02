#pragma once
#include <algorithm>
#include <random>
#include <functional>

namespace BeastEngine
{
	struct Mathf
	{
		//start��end��t�Ő��`��Ԃ���
		static float Lerp(float start, float end, float t)
		{
			return (1 - t) * start + t * end;
		}

		//�w�肵���m��(���P��)��True��Ԃ�
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

		//min��max�̊ԂŃ����_���Ȓl��Ԃ�(max�͊܂܂�Ȃ����Ƃɒ���)
		static float Random_Range(float min, float max)
		{
			float f = min + static_cast <float> (std::rand()) / (static_cast <float> (RAND_MAX / (max - min)));
			return f;
		}

		//Value��Min��Max�̊ԂɃN�����v����
		static float Clamp(float Value, float Min, float Max)
		{
			float f = (std::min)((std::max)(Value, Min), Max);
			return f;
		}

		//Float�덷���l�������l���ǂ�����Ԃ�
		static bool Approximately(float x, float y, const float epsilon = FLT_EPSILON)
		{
			if (std::fabsf(x - y) < epsilon)return true;
			return false;
		}
	};
}