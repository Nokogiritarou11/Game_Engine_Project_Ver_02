#pragma once
#include <algorithm>
#include <random>

namespace BeastEngine
{
	//����Z�p�֐�
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

		//start��end��t�Ő��`��Ԃ���
		static float Lerp(const float start, const float end, const float t)
		{
			return (1 - t) * start + t * end;
		}

		//�w�肵���m��(���P��)��True��Ԃ�
		static bool Probability(const float f_percent)
		{
			if (const std::uniform_real_distribution<> dist(0, 100); dist(gen) < f_percent)
			{
				return true;
			}

			return false;
		}

		//min��max�̊ԂŃ����_���Ȓl��Ԃ�
		static float Random_Range(const float min, const float max)
		{
			const std::uniform_real_distribution<> dist(min, max);
			return static_cast<float>(dist(gen));
		}

		//min��max�̊ԂŃ����_���Ȓl��Ԃ�
		static int Random_Range(const int min, const int max)
		{
			const std::uniform_int_distribution<> dist(min, max);
			return dist(gen);
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