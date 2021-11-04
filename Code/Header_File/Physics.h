#pragma once
#include "Original_Math.h"

namespace BeastEngine
{
	//物理系ヘルパー関数
	class Physics
	{
	public:
		static bool Raycast(const Vector3& from, const Vector3& to); //コライダーに対しレイキャストを行う
	};
}