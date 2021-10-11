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
		Knock_Down,
		Up,
		Down
	};

	class Interface_Character_Damageable
	{
	public:
		virtual void Take_Damage(const int& damage_hp, const int& damage_stun, const Vector3& from_pos, const Damage_Type& damage_state) = 0;

	private:
	};
}