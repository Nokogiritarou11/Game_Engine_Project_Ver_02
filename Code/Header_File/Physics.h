#pragma once
#include "Original_Math.h"

namespace BeastEngine
{
	//�����n�w���p�[�֐�
	class Physics
	{
	public:
		static bool Raycast(const Vector3& from, const Vector3& to, Vector3& hit_pos_res, int layer_mask = 1); //�R���C�_�[�ɑ΂����C�L���X�g���s��
	};
}