#pragma once
#include "Original_Math.h"

namespace BeastEngine
{
	enum class Damage_Type
	{
		Front,
		Back,
		Right,
		Left,
		Push,
		Knock_Down,
		Up,
		Down
	};

	class Interface_Character_Damageable
	{
	public:
		virtual ~Interface_Character_Damageable() = default;
		virtual bool Take_Damage(int damage_hp, int damage_stun, const std::shared_ptr<Transform>& from_transform, Damage_Type damage_state) = 0;
	};
}