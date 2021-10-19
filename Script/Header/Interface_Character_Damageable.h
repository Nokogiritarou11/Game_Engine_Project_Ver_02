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
		virtual void Take_Damage(int damage_hp, int damage_stun, Vector3 from_pos, Damage_Type damage_state) = 0;
	};
}