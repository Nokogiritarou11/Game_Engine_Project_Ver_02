#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter
	{
	public:
		float max_hp = 0;
		float hp = 0;

		float max_stun = 0;
		float stun = 0;
		float heal_stun = 0;

		float max_just_guard_time = 0;
		float just_guard_timer = 0;

		bool pausing = false;
		bool living = true;
		bool moving = false;
		bool attacking = false;
		bool damaging = false;
		bool guarding = false;

		bool is_ground = false;
		bool is_invincible = false;

	protected:
		void Character_Parameter_Reset()
		{
			hp = max_hp;
			stun = max_stun;
			just_guard_timer = 0;

			pausing = false;
			living = true;
			moving = false;
			attacking = false;
			damaging = false;
			guarding = false;
			is_invincible = false;
		}

	private:
		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(max_hp, max_stun, heal_stun, max_just_guard_time);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Character_Parameter, 1)