#pragma once
#include <algorithm>
#include <random>
#include <functional>

namespace BeastEngine
{
	struct Mathf
	{
		//start��end��t�Ő��`��Ԃ���
		static float Lerp(const float start, const float end, const float t)
		{
			return (1 - t) * start + t * end;
		}

		//�w�肵���m��(���P��)��True��Ԃ�
		static bool Probability(float f_percent)
		{
			f_percent /= 100;
			if (static_cast<float>(std::rand()) / RAND_MAX < f_percent)
			{
				return true;
			}

			return false;
		}

		//min��max�̊ԂŃ����_���Ȓl��Ԃ�(max�͊܂܂�Ȃ����Ƃɒ���)
		static float Random_Range(const float min, const float max)
		{
			return min + static_cast <float> (std::rand()) / (RAND_MAX / (max - min));
		}

		//Value��Min��Max�̊ԂɃN�����v����
		static float Clamp(const float value, const float min, const float max)
		{
			return (std::min)((std::max)(value, min), max);
		}

		//Float�덷���l�������l���ǂ�����Ԃ�
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