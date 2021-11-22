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

		bool pausing = false;
		bool living = true;
		bool moving = false;
		bool attacking = false;
		bool damaging = false;
		bool guarding = false;
		bool just_guarding = false;

		bool is_ground = true;
		bool is_invincible = false;

	private:
		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(max_hp, max_stun, heal_stun);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Character_Parameter, 1)