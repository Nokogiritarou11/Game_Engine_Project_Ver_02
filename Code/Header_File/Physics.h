#pragma once
#include "Original_Math.h"

namespace BeastEngine
{
	class Physics
	{
	public:
		static bool Raycast(const BeastEngine::Vector3& from, const BeastEngine::Vector3& to);
	};
}